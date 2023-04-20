//
// Created by Robert Sale on 4/17/23.
//

#include "Socket.h"

const std::regex LibSocket::ipv4_regex{"(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])"};
const std::regex LibSocket::ipv6_regex{"([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}"};
const std::regex LibSocket::port_regex{"([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])"};

namespace LibSocket {

} // LibSocket