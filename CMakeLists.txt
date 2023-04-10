cmake_minimum_required(VERSION 3.25)
project(ClientServerChatApp)

set(CMAKE_CXX_STANDARD 20)

set(SHARED_SRCS
        SocketHelpers.h
        Printer.h
        SocketHelpers.cpp
        SocketEntity.cpp
        SocketEntity.h
        Commands.h
        Console.cpp
        Console.h SyncPoint.h)

set(CLIENT_SRCS clientmain.cpp Client.cpp Client.h)
set(SERVER_SRCS clientmain.cpp Server.cpp Server.h)


add_executable(client ${CLIENT_SRCS} ${SHARED_SRCS})
target_precompile_headers(client PUBLIC pch.h)

add_executable(server ${SERVER_SRCS} ${SHARED_SRCS})
target_precompile_headers(server PUBLIC pch.h)
