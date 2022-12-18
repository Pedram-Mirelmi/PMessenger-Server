cmake_minimum_required(VERSION 3.15)
project(PMessenger_Server)

set(CMAKE_CXX_STANDARD 20)

add_executable(PMessenger_Server
        KeyWords.hpp
        main.cpp

        # Async Server Framework
        AsyncServerFramework/AbstractAsyncServer.h
        AsyncServerFramework/BasicNetIOManager.h
        AsyncServerFramework/INetMessageProcessor.h
        AsyncServerFramework/INetWriter.h
        AsyncServerFramework/IService.h
        AsyncServerFramework/Session.h
        AsyncServerFramework/io/BasicNetMessage.h
        AsyncServerFramework/io/INetMessageBodyDeserializer.h
        AsyncServerFramework/io/ISerializable.h


        include/Server.hpp
        include/Server.cpp
        include/NetIO/NetMessages.hpp
        include/NetIO/NetMessages.cpp
        include/NetIO/MessageBodyDeserializer.hpp
        include/NetIO/MessageBodyDeserializer.cpp
        include/DataHandling/DatabaseConnector.hpp
        include/DataHandling/DataBaseConnector.cpp

)

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


