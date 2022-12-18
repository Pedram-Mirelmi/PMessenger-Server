#include "./NetMessages.hpp"
#include <string.h>

const NetMessageType &LoginMessage::getMessageType() const {
    return NetMessageType::LOGIN;
}

void LoginMessage::deserialize(char *buffer) {
    this->m_header.deserialize(buffer);
    buffer += m_header.calculateNeededSizeForThis();

    uint8_t usernameSize = *(reinterpret_cast<uint8_t*>(buffer));
    buffer += sizeof (uint8_t);

    this->m_username = std::string(buffer, usernameSize);
    buffer += usernameSize;

    uint8_t passwordSize = *(reinterpret_cast<uint8_t*>(buffer));
    buffer += sizeof (uint8_t);

    this->m_password = std::string(buffer, usernameSize);
}

void LoginMessage::serialize(char *buffer) const
{
    // header
    m_header.serialize(buffer);
    buffer += m_header.calculateNeededSizeForThis();

    // username size
    *(reinterpret_cast<uint8_t*>(buffer)) = (uint8_t)m_username.size();
    buffer += sizeof (uint8_t);

    // username string
    memcpy(buffer, m_username.data(), m_username.size());
    buffer += m_username.size();

    // password size
    *(reinterpret_cast<uint8_t*>(buffer)) = (uint8_t)m_username.size();
    buffer += sizeof (uint8_t);

    // password string
    memcpy(buffer, m_password.data(), m_password.size());
}

uint32_t LoginMessage::calculateNeededSizeForThis() const {
    return m_header.calculateNeededSizeForThis()
           + m_username.size()
           + sizeof (uint8_t)
           + m_password.size()
           + sizeof (uint8_t);
}




void SignUpMessage::deserialize(char *buffer)
{
    this->m_header.deserialize(buffer);
    buffer += m_header.calculateNeededSizeForThis();

    uint8_t usernameSize = *(reinterpret_cast<uint8_t*>(buffer));
    buffer += sizeof (uint8_t);

    this->m_username = std::string(buffer, usernameSize);
    buffer += usernameSize;

    uint8_t passwordSize = *(reinterpret_cast<uint8_t*>(buffer));
    buffer += sizeof (uint8_t);

    this->m_password = std::string(buffer, usernameSize);
}

void SignUpMessage::serialize(char *buffer) const
{
    // header
    m_header.serialize(buffer);
    buffer += m_header.calculateNeededSizeForThis();

    // username size
    *(reinterpret_cast<uint8_t*>(buffer)) = (uint8_t)m_username.size();
    buffer += sizeof (uint8_t);

    // username string
    memcpy(buffer, m_username.data(), m_username.size());
    buffer += m_username.size();

    // password size
    *(reinterpret_cast<uint8_t*>(buffer)) = (uint8_t)m_username.size();
    buffer += sizeof (uint8_t);

    // password string
    memcpy(buffer, m_password.data(), m_password.size());
}

uint32_t SignUpMessage::calculateNeededSizeForThis() const
{
    return m_header.calculateNeededSizeForThis()
           + m_username.size()
           + sizeof (uint8_t)
           + m_password.size()
           + sizeof (uint8_t);
}

const NetMessageType &SignUpMessage::getMessageType() const
{
    return NetMessageType::SIGN_UP;
}

