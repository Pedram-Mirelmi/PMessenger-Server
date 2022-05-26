#pragma once
#include <vector>
#include <thread>
// #include <stdlib.h>
#include <memory>
#include <mutex>
#include <unordered_set>

#include <jsoncpp/json/json.h>
#include "./NetworkHandler.hpp"
#include "ServerRequestHandler.hpp"

#define INVALID_ID -1

class ClientConnection
{
typedef std::unordered_map<id_T, std::shared_ptr<ClientConnection>> conn_set;
private:
    NetworkHandler network_handler;
    id_T user_id = INVALID_ID;
    conn_set& other_connections;
    JsonObj current_request;
    std::vector<JsonObj> current_responses;
    std::chrono::steady_clock::time_point last_req_time;
    ServerRequestHandler rh;
public:
    static std::chrono::steady_clock::duration maxSuspension;
   
    ClientConnection(const int& socket, conn_set& others)
        : network_handler(socket), other_connections(others), rh(user_id, network_handler)
    {};

    id_T handleLoginOrRegister()
    { 
        using namespace KeyWords;
        try
        {   
            while (true)
            {
                current_responses.clear();
                current_request.clear();
                this->network_handler.receiveMessage(current_request);
                if (current_request[NET_MESSAGE_TYPE] == CLOSE_CONNECTION)
                {
                    std::cout << "connection closed properly by client" << std::endl;
                    return INVALID_ID;
                }
                current_responses = this->rh.handle(current_request);
//                std::cout << current_responses.size();
                if ((current_request[NET_MESSAGE_TYPE].asString() == REGISTER
                     || current_request[NET_MESSAGE_TYPE].asString() == LOGIN)
                    && current_responses[0][SUCCESSFUL].asBool())
                {
                    this->user_id = std::stoi(current_responses[0][USER_INFO][USER_ID].asString());
                    this->network_handler.sendMessage(current_responses[0]);
                    break;
                }
                this->network_handler.sendMessage(current_responses[0]);
            }
            // std::thread(&ClientConnection::controlOnlineTime, this).detach();
            return this->user_id;
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            return INVALID_ID;
        }
    }

    void talk()
    {
        using namespace KeyWords;
        try
        {
            while (true)
            {
                current_responses.clear();
                current_request.clear();
                this->network_handler.receiveMessage(current_request);
                if (current_request[NET_MESSAGE_TYPE].asString() == CLOSE_CONNECTION)
                {
                    std::cout << "connection closed properly by client" << std::endl;
                    return;
                }
                // std::thread(&ClientConnection::controlOnlineTime, this).detach();
                current_responses = this->rh.handle(current_request);
                for(const auto& response: current_responses) {
                    this->network_handler.sendMessage(response);
                }
                // std::thread(&ClientConnection::checkForSpecialOperations, this, current_request, current_responses).detach();

            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

private:
    void controlOnlineTime()
    {
        if (std::chrono::steady_clock::now() - this->last_req_time > ClientConnection::maxSuspension)
            this->rh.setMeOnline(this->user_id);
        this->last_req_time = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(ClientConnection::maxSuspension);
        if (std::chrono::steady_clock::now() - this->last_req_time > ClientConnection::maxSuspension)
            this->rh.setMeOffline(this->user_id);
    }

    void checkForSpecialOperations(const msg_t& request, const msg_t& response)
    {
        using namespace KeyWords;
        if (request[NET_MESSAGE_TYPE] == NEW_TEXT_MESSAGE
            && response[SUCCESSFUL].asBool())
        {
            this->announceNewMessageToOthers(response[MESSAGE_INFO]);
        }
    }

    void announceNewMessageToOthers(const msg_t& message_info)
    {
        using namespace KeyWords;
        std::unordered_set<std::string> audiences;
        this->rh.getParticipants(message_info[ENV_ID].asInt64(), audiences);
        std::vector<std::thread> threads;
        threads.reserve(audiences.size());
        for (const std::string& id_str : audiences)
            threads.emplace_back([&]() 
                {
                    auto audience = this->other_connections[std::stoi(id_str)];
                    if (audience != nullptr)
                        audience->announceNewMessageToMe(current_request);
                }
            );
        for (auto& thread : threads)
            thread.join();
   }

public:
    void announceNewMessageToMe(const msg_t& message_info)
    {
        this->network_handler.sendMessage(message_info);
    }
};


