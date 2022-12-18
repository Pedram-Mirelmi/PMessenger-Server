#include "Server.hpp"
#include "DataHandling/DatabaseConnector.hpp"

Server::Server(const std::string &ip, uint16_t port, int netIOThreadsCount)
    : AbstractAsyncServer<NetMessageType>(std::move(ip), std::move(port), std::move(netIOThreadsCount)),
      INetMessageProcessor<NetMessageType>()
{
    m_netIOManager->setMessageProcessor(shared_ptr<INetMessageProcessor<NetMessageType>>(this));
    m_netIOManager->setBodyDeserializer(
                shared_ptr<INetMessageBodyDeserializer<NetMessageType>>(
                    (INetMessageBodyDeserializer<NetMessageType>*) new MessageBodyDeserializer()));
}

void Server::processNetMessage(shared_ptr<BasicNetMessage<NetMessageType>> netMsg,
                               shared_ptr<Session<NetMessageType>> session)
{
    switch (netMsg->getMessageType())
    {
        case NetMessageType::LOGIN:
            break;
        case NetMessageType::SIGN_UP:
            break;
        case NetMessageType::FETCH:
            break;

    }
}

