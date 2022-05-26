#pragma once
#include <fmt/format.h>
#include "../KeyWords.hpp"
#include "DatabaseConnector.hpp"
#include <unordered_set>
#include "../stringTools.hpp"
#include <string>
#include "./NetworkHandler.hpp"

typedef Json::Value JsonObj;
typedef unsigned long long int id_T;
typedef std::unordered_set<std::string> set;
typedef Json::Value JsonObj;

class ServerRequestHandler
{
private:
    id_T& m_user_id;
    NetworkHandler& network_handler;
public:
    DBConnector m_db;
    explicit ServerRequestHandler(id_T& user_id, NetworkHandler& net_handler)
        : m_user_id(user_id), m_db(user_id), network_handler(net_handler)
    {}


    std::vector<JsonObj> handle(JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses;
        const std::string &net_msg_type = request[NET_MESSAGE_TYPE].asString();
        std::cout << "net_msg_type: " << net_msg_type << std::endl;
        if (net_msg_type == REGISTER) {
            return this->handleRegister(request); // std::move avoid copying
        }
        if (net_msg_type == LOGIN)
        {
            return this->handleLogin(request); // std::move avoid copying
        }
        if (net_msg_type == FETCH) {
            return this->handleFetch(request);
        }
        if (net_msg_type == GET_PRIVATE_ENV_DETAILS) {
            return this->handleGetPrivateEnvDetails(request);
        }
        if(net_msg_type == GET_PRIVATE_ENV_MESSAGES){
            return this->handleGetPrivateEnvMessages(request);
        }
        if (net_msg_type == GET_USER_INFO) {
            return this->handleGetUserInfo(request);
        }
        if (net_msg_type == SEARCH_USERNAME) {
            return this->handleSearchUsername(request); // std::move avoid copying
        }
        if (net_msg_type == NEW_TEXT_MESSAGE) {
            return this->handleNewTextMessage(request);
        }
        if (net_msg_type == CREATE_NEW_PRIVATE_CHAT) {
            return this->handleCreateNewPrivateChat(request);
        }


        std::cerr << "Invalid request: " << request << std::endl;
        return std::vector<JsonObj> (1);
    }

    void setMeOffline(const id_t& user_id)
    {

    } 
    
    void setMeOnline(const id_t& user_id)
    {

    }
    
    void getParticipants(const int64_t& env_id_str, set& participants)
    {

    }

    std::vector<JsonObj> handleRegister(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        JsonObj& response = responses[0];
        response[NET_MESSAGE_TYPE] = REGISTER_RESULT;
        auto created_user_id = this->m_db.insertNewUser(request[USERNAME].asCString(),
                                                        request[PASSWORD].asCString());
        if (created_user_id)
        {
            auto Qry = fmt::format("SELECT * FROM users WHERE user_id = {};", created_user_id);
            this->m_db.singleSELECT(Qry, response[USER_INFO]);
            response[SUCCESSFUL] = true;
            return responses;
        }
        response[SUCCESSFUL] = false;
        return responses;
    }

    std::vector<JsonObj> handleLogin(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        response[NET_MESSAGE_TYPE] = LOGIN_RESULT;
        auto Qry = fmt::format("SELECT * FROM users WHERE username = '{}';",
                                             request[USERNAME].asCString());
        this->m_db.singleSELECT(Qry, response[USER_INFO]);
        if (response[USER_INFO][PASSWORD].asString() == request[PASSWORD].asString())
        {
            response[SUCCESSFUL] = true;
            return responses;
        }
        response.clear();
        response[SUCCESSFUL] = false;
        return responses;
    }

    std::vector<JsonObj> handleFetch(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        response[NET_MESSAGE_TYPE] = DATA;
        response[DATA_TYPE] = FETCH_RESULT;
        this->m_db.getPrivateEnvsThatUserAttends(response[PRIVATE_CHATS], this->m_user_id);
        return responses;
    }


    std::vector<JsonObj> handleGetPrivateEnvDetails(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(2);

        auto& env_details_part = responses[0];
        env_details_part[NET_MESSAGE_TYPE] = DATA;
        env_details_part[DATA_TYPE] = PRIVATE_ENV_DETAILS;
        this->m_db.getPrivateEnvDetails(env_details_part[ENV_INFO] , request[ENV_ID].asUInt64());
        this->m_db.getUserInfoByUserId(env_details_part[OTHER_PERSON_INFO],
                                       env_details_part[ENV_INFO][OTHER_PERSON_ID].asUInt64());

        auto& env_messages_part = responses[1];
        env_messages_part[NET_MESSAGE_TYPE] = DATA;
        env_messages_part[DATA_TYPE] = MESSAGE;
        this->m_db.getPrivateEnvMessages(env_messages_part, request[ENV_ID].asUInt64());

        return responses;
    }

    std::vector<JsonObj> handleGetUserInfo(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        response[NET_MESSAGE_TYPE] = DATA;
        response[DATA_TYPE] = USER_INFO;
        this->m_db.getUserInfoByUserId(response[USER_INFO], request[USER_ID].asUInt());
        response[USER_INFO].removeMember(PASSWORD);
        return responses;
    }
    std::vector<JsonObj> handleGetPrivateEnvMessages(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        this->m_db.getPrivateEnvMessages(response, request[ENV_ID].asUInt64(), request[LAST_MESSAGE_CLIENT_HAS].asUInt64());
        return responses;
    };

    std::vector<JsonObj> handleSearchUsername(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        auto Qry = fmt::format("SELECT user_id, username, name, created_at"
                               " FROM users WHERE username LIKE '%{}%';",
                                request[USERNAME_TO_SEARCH].asCString());
        JsonArr search_result;
        this->m_db.SELECT(Qry, search_result);
        response[SEARCH_RESULT] = search_result;
        response[SUCCESSFUL] = true;
        return responses;
    }

    std::vector<JsonObj> handleCreateNewPrivateChat(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        auto created_env_id = this->m_db.insertPrivateChatBetween(this->m_user_id,
                                                                  request[USER_ID].asUInt64());
        if (created_env_id)
        {
            auto get_info_query = fmt::format("SELECT * FROM private_chats_view "
                                              "WHERE env_id = {}", created_env_id);
            this->m_db.singleSELECT(get_info_query, response[ENV_INFO]);
            response[ENV_INFO][ENV_TYPE] = PRIVATE_CHAT;
            response[ENV_INFO][INVALID_ENV_ID] = std::to_string(request[INVALID_ENV_ID].asUInt64());
            response[SUCCESSFUL] = true;
            return responses;
        }
        response[SUCCESSFUL] = false;
        return responses;
    }

    std::vector<JsonObj> handleNewTextMessage(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        auto created_message_id = this->m_db.insertNewTextMessage(this->m_user_id,
                                                                  request[ENV_ID].asUInt64(),
                                                                  request[MESSAGE_TEXT].asCString());
        if (created_message_id)
        {
            auto get_info_query = fmt::format("SELECT * FROM text_messages_view "
                                              "WHERE message_id = {};", created_message_id);
            this->m_db.singleSELECT(get_info_query, response[MESSAGE_INFO]);
            response[SUCCESSFUL] = true;
            return responses;
        }
        response[SUCCESSFUL] = false;
        return responses;
    }

    void addContact(const JsonObj& request)
    {
        using namespace KeyWords;
        std::vector<JsonObj> responses(1);
        auto& response = responses[0];
        auto query = fmt::format("INSERT INTO contacts(user_id, contact_id, name_saved) "
                            "VALUES ({}, {}, '{}');",
                            request[USER_ID].asString(),
                            request[CONTACT_ID].asString(),
                            request[NAME_SAVED].asString());
        
        response[SUCCESSFUL] = this->m_db.execTransactionQuery(query);
    }

    void getContacts(JsonObj& request, JsonObj& response) // TODO
    {
        
    }

};

