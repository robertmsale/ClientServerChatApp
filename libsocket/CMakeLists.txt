cmake_minimum_required(VERSION 3.25)
project(ClientServerChatApp)

# uses C++ 20 which is well supported at this time
set(CMAKE_CXX_STANDARD 20)

set(SRCS
        Socket.cpp
        Socket.h
        Errors.h
        pch.h
        )

add_library(libsocket ${SRCS})
target_precompile_headers(libsocket PUBLIC pch.h)