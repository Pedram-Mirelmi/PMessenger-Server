#pragma once
#include <asio.hpp>
#include <iostream>
#include <mutex>

#include "io/BasicNetMessage.h"

using std::string;

template <typename MsgType>
class Session : public std::enable_shared_from_this<Session<MsgType>>
{
    using socket = asio::ip::tcp::socket;


    socket m_socket;
    std::vector<char> m_headerInBuffer;
    std::vector<char> m_bodyInBuffer;
    NetMessageHeader<MsgType> m_tempHeader;
public:
    Session(socket&& socket)
        :m_socket(std::move(socket))
    {
        m_headerInBuffer.resize(sizeof(NetMessageHeader<MsgType>));
    }

    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;

    virtual void deserializeHeader()
    {
        m_tempHeader.deserialize(m_headerInBuffer.data());
        m_bodyInBuffer.resize(m_tempHeader.getBodySize());
    }

public:

    socket& getSocket()
    {
        return m_socket;
    }
    char *getHeaderInBuffer()
    {
        return m_headerInBuffer.data();
    }
    char *getBodyInBuffer()
    {
        return m_bodyInBuffer.data();
    }

    NetMessageHeader<MsgType> &getTempHeader()
    {
        return m_tempHeader;
    }

};

