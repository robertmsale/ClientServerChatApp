//
// Created by Robert Sale on 4/10/23.
//
#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <future>
#include <iostream>
#include <limits>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>
#include <map>
#include "RingBuffer.h"
#include "DeferExec.h"

/**
 * Much needed shortcut for unique_locks
 */
using UniqueLock = std::unique_lock<std::mutex>;

/**
 * The data type used by POSIX to describe the socket for clarity
 */
using SocketFileDescriptor = int;

/**
 * Data type of the socket size. This determines how large the initial message size part of the message will be.
 */
using SocketSizeType = char;

/**
 * Evaluates to how large the buffer will be for messages. This is dynamically evaluated at compile time based
 * on the SocketSizeType and allows for flexibility.
 * @return maximum size of message including first bytes describing actual size of payload
 */
constexpr size_t SocketMaxMessageSize() {
    size_t size = sizeof(SocketSizeType);
    for (size_t i = 1; i <= sizeof(SocketSizeType) * 8; ++i) {
        size *= 2;
    }
    return size;
}

