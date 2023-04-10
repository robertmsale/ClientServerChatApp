//
// Created by Robert Sale on 4/10/23.
//

#ifndef CLIENTSERVERCHATAPP_PCH_H
#define CLIENTSERVERCHATAPP_PCH_H

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

using UniqueLock = std::unique_lock<std::mutex>;

#endif //CLIENTSERVERCHATAPP_PCH_H
