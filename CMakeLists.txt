cmake_minimum_required(VERSION 3.20)
project(ClientServerChatApp)

# uses C++ 20 which is well supported at this time
set(CMAKE_CXX_STANDARD 20)

# sources shared by client and server
set(SHARED_SRCS
        Commands.h
        Console.cpp
        Console.h
        SyncPoint.h
        RingBuffer.h
        pch.h
        ShutdownTasks.cpp
        ShutdownTasks.h)

# Client only sources
set(CLIENT_SRCS clientmain.cpp Client.cpp Client.h)
# Server only sources
set(SERVER_SRCS servermain.cpp Server.cpp Server.h Logger.cpp Logger.h)

add_subdirectory(libsocket)

# create client executable
add_executable(client ${CLIENT_SRCS} ${SHARED_SRCS})
# insert precompiled header into client sources
target_precompile_headers(client PUBLIC pch.h)
target_link_libraries(client PUBLIC libsocket)


# create server executable
add_executable(server ${SERVER_SRCS} ${SHARED_SRCS})
# insert precompiled header into server sources
target_precompile_headers(server PUBLIC pch.h)
target_link_libraries(server PUBLIC libsocket)
