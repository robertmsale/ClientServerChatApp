cmake_minimum_required(VERSION 3.25)
project(ClientServerChatApp)

# uses C++ 20 which is well supported at this time
set(CMAKE_CXX_STANDARD 20)

# sources shared by client and server
set(SHARED_SRCS
        SocketEntity.cpp
        SocketEntity.h
        Commands.h
        Console.cpp
        Console.h
        SyncPoint.h
        RingBuffer.h
        )

# Client only sources
set(CLIENT_SRCS clientmain.cpp Client.cpp Client.h Logger.cpp Logger.h)
# Server only sources
set(SERVER_SRCS servermain.cpp Server.cpp Server.h Logger.cpp Logger.h)

# create client executable
add_executable(client ${CLIENT_SRCS} ${SHARED_SRCS})
# insert precompiled header into client sources
target_precompile_headers(client PUBLIC pch.h)


# create server executable
add_executable(server ${SERVER_SRCS} ${SHARED_SRCS})
# insert precompiled header into server sources
target_precompile_headers(server PUBLIC pch.h)
