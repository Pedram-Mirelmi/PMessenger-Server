#pragma once

#include "../../AsyncServerFramework/io/BasicNetMessage.h"


enum class NetMessageType : uint8_t
{
    LOGIN = 1,
    SIGN_UP,
    FETCH
};


class LoginMessage : public BasicNetMessage<NetMessageType>
{
public:
    std::string m_username, m_password;

    void deserialize(char *buffer) override;
    void serialize(char *buffer) const override;
    uint32_t calculateNeededSizeForThis() const override;

    // BasicNetMessage interface
public:
    const NetMessageType &getMessageType() const override;
};

class SignUpMessage : public BasicNetMessage<NetMessageType>
{
public:
    std::string m_username, m_password;



    // ISerializable interface
public:
    void deserialize(char *buffer) override;
    void serialize(char *buffer) const override;
    uint32_t calculateNeededSizeForThis() const override;

    // BasicNetMessage interface
public:
    const NetMessageType &getMessageType() const override;
};
