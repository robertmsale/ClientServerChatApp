cmake_minimum_required(VERSION 3.20)
project(ClientServerChatApp)

# uses C++ 20 which is well supported at this time
set(CMAKE_CXX_STANDARD 20)

set(SRCS
        Socket.cpp
        Socket.h
        Errors.h
        )

add_library(libsocket ${SRCS})