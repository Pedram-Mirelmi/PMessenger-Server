#pragma once
#include <memory>
#include <vector>
#include "IService.h"
#include "./BasicNetIOManager.h"

using std::shared_ptr;

template <typename MsgType>
class AbstractAsyncServer : IService, std::enable_shared_from_this<AbstractAsyncServer<MsgType>>
{
protected:
    shared_ptr<BasicNetIOManager<MsgType>> m_netIOManager;
    std::vector<shared_ptr<IService>> m_services;
public:
    AbstractAsyncServer(const std::string& ip, uint16_t port, const int& netIOThreadsCount)
        : IService(), m_netIOManager(make_shared<BasicNetIOManager<MsgType>>(ip, port, netIOThreadsCount))
    {
        m_services.push_back(m_netIOManager);
    }


    // IService interface
public:
    virtual void start() override
    {
        for (auto& service : m_services)
            service->start();
    }
    virtual void stop() override
    {
        for (auto& service : m_services)
            service->stop();
    }
};
