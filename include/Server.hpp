#pragma once

#include "../AsyncServerFramework/AbstractAsyncServer.h"
#include "./NetIO/NetMessages.hpp"
#include "./NetIO/MessageBodyDeserializer.hpp"

class Server : public AbstractAsyncServer<NetMessageType>,
               public INetMessageProcessor<NetMessageType>,
               public std::enable_shared_from_this<Server>
{
public:
    Server(const std::string& ip, uint16_t port, int netIOThreadsCount);
    void processNetMessage(shared_ptr<BasicNetMessage<NetMessageType>> netMsg, shared_ptr<Session<NetMessageType>> session) override;

};
