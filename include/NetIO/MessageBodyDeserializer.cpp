#include "MessageBodyDeserializer.hpp"

MessageBodyDeserializer::MessageBodyDeserializer()
    : INetMessageBodyDeserializer<NetMessageType>()
{

}

shared_ptr<BasicNetMessage<NetMessageType> > MessageBodyDeserializer::deserializeBody(NetMessageHeader<NetMessageType> header, const char *bodyBuffer)
{

}
