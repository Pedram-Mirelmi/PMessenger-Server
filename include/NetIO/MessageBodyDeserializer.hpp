#pragma once


#include "../../AsyncServerFramework/io/INetMessageBodyDeserializer.h"
#include "./NetMessages.hpp"

class MessageBodyDeserializer : public INetMessageBodyDeserializer<NetMessageType>
{
public:
    MessageBodyDeserializer();


    // INetMessageBodyDeserializer interface
public:
    shared_ptr<BasicNetMessage<NetMessageType>>
    deserializeBody(NetMessageHeader<NetMessageType> header, const char *bodyBuffer) override;
};
