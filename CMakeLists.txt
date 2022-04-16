cmake_minimum_required(VERSION 3.17)
project(PMessenger_Server)

set(CMAKE_CXX_STANDARD 20)

add_executable(PMessenger_Server Server.cpp ServerPackages/ClientConnection.hpp ServerPackages/DatabaseConnector.hpp ServerPackages/NetworkHandler.hpp KeyWords.hpp)

find_package(fmt)
target_link_libraries(PMessenger_Server fmt)

find_package(jsoncpp)
target_link_libraries(PMessenger_Server jsoncpp)

find_package(pthread)
target_link_libraries(PMessenger_Server pthread)

find_package(pqxx)
target_link_libraries(PMessenger_Server pqxx)

find_package(pq)
target_link_libraries(PMessenger_Server pq)

