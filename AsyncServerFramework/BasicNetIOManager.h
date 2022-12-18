#pragma once

#include "./IService.h"
#include "./INetMessageProcessor.h"
#include "./io/BasicNetMessage.h"
#include "./INetWriter.h"
#include "./io/INetMessageBodyDeserializer.h"

using std::shared_ptr;
using std::make_shared;


template<typename MsgType>
class BasicNetIOManager : public IService,
                          public INetWriter<MsgType>,
                          public std::enable_shared_from_this<BasicNetIOManager<MsgType>>
{
    using socket = asio::ip::tcp::socket;
protected:
    std::vector<std::thread> m_ioThreads;
    asio::io_context m_ioContext;

    asio::ip::tcp::acceptor m_acceptor;

    shared_ptr<INetMessageProcessor<MsgType>> m_messageProcessor;
    shared_ptr<INetMessageBodyDeserializer<MsgType>> m_bodyDeserializer;
public:
    BasicNetIOManager(const string& ip, const uint16_t port, const int& ioThreadsCount)
        : IService(), INetWriter<MsgType>(), m_acceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::make_address(ip), port))
    {
        m_ioThreads.reserve(ioThreadsCount);
    }
    ~BasicNetIOManager()
    {
        this->stop();
    }

    virtual void onAsyncAccepted(std::error_code ec, socket socket)
    {
        m_acceptor.async_accept(std::bind(&BasicNetIOManager::onAsyncAccepted,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        if(!ec)
        {
            std::cout << "New Connection accepted" << std::endl;
            shared_ptr<Session<MsgType>> newConnection = make_shared<Session<MsgType>>(std::move(socket));
            asio::async_read(newConnection->getSocket(),
                             asio::buffer(newConnection->getHeaderInBuffer(), NetMessageHeader<MsgType>::getHeaderSize()),
                             std::bind(&BasicNetIOManager::onAsyncReadHeader,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2,
                                       newConnection)
                             );



        }
        else
        {

        }
    }

    virtual void onAsyncReadHeader(std::error_code ec, std::size_t length, shared_ptr<Session<MsgType>> session)
    {
        if(!ec)
        {
            session->deserializeHeader();
            asio::async_read(session->getSocket(),
                             asio::buffer(session->getBodyInBuffer(), session->getTempHeader().getBodySize()),
                             std::bind(&BasicNetIOManager::onAsyncReadBody,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2,
                                       session)
                             );
        }
        else
        {

        }
    }

    virtual void onAsyncReadBody(std::error_code ec, std::size_t length, shared_ptr<Session<MsgType>> session)
    {
        if(!ec)
        {
            auto netMessage = m_bodyDeserializer->deserializeBody(session->getTempHeader(), session->getBodyInBuffer());
            m_messageProcessor->processNetMessage(std::move(netMessage), session);
            asio::async_read(session->getSocket(),
                             asio::buffer(session->getHeaderInBuffer(), NetMessageHeader<MsgType>::getHeaderSize()),
                             std::bind(&BasicNetIOManager::onAsyncReadHeader,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2,
                                       session)
                             );
        }
    }

    virtual void onAsyncWrite(std::error_code ec, std::size_t length)
    {
        if(!ec)
        {
            // All good
        }
        else
        {

        }
    }

    virtual void writeMessage(shared_ptr<BasicNetMessage<MsgType>> msg, shared_ptr<Session<MsgType>> session) override
    {
        uint32_t msgSize = msg->getHeader().getBodySize() + msg->getHeader().calculateNeededSizeForThis();
        char* msgBuffer = new char[msgSize];
        msg->serialize(msgBuffer);
        asio::async_write(session->getSocket(),
                          asio::buffer(msgBuffer, msgSize),
                          std::bind(&BasicNetIOManager::onAsyncWrite,
                          this,
                          std::placeholders::_1,
                          std::placeholders::_2)
                          );
    }

    // IService interface
public:
    virtual void start() override
    {
        m_acceptor.async_accept(std::bind(&BasicNetIOManager::onAsyncAccepted,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        for(uint32_t i = 0; i < m_ioThreads.capacity(); i++)
        {
            this->m_ioThreads.emplace_back([this](){
                this->m_ioContext.run();
            });
        }
    }
    virtual void stop() override
    {
        m_ioContext.stop();
        for (auto& thread : m_ioThreads)
            if(thread.joinable())
                thread.join();
    }

    void setMessageProcessor(const shared_ptr<INetMessageProcessor<MsgType>> &newMessageProcessor)
    {
        m_messageProcessor = newMessageProcessor;
    }
    void setBodyDeserializer(const shared_ptr<INetMessageBodyDeserializer<MsgType> > &newBodyDeserializer)
    {
        m_bodyDeserializer = newBodyDeserializer;
    }
};




