//
// Created by Robert Sale on 4/17/23.
//

#include "Socket.h"

const std::regex LibSocket::ipv4_regex{"(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])"};
const std::regex LibSocket::ipv6_regex{"([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}"};
const std::regex LibSocket::port_regex{"([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])"};

namespace LibSocket {
    void Socket::accept(struct sockaddr* address, socklen_t* len) {
        auto result = ::accept(_fd, address, len);
        auto err = errno;
        if (result == -1 && accept_handlers.contains((SocketAcceptError)err)) {
            accept_handlers[(SocketAcceptError)err]();
        }
    }
    void Socket::accept() {
        accept(nullptr, nullptr);
    }
    void Socket::bind(const struct sockaddr* address, socklen_t address_len) {
        auto result = ::bind(_fd, address, address_len);
        auto err = errno;
        if (result == -1 && bind_handlers.contains((SocketBindError)err)) {
            bind_handlers[(SocketBindError)err]();
        }
    }
    void Socket::bind(const struct sockaddr_in* address) {connect((struct sockaddr*)address, sizeof(*address));}
    void Socket::bind(const struct sockaddr_in6* address) {connect((struct sockaddr*)address, sizeof(*address));}
    void Socket::close() {
        auto result = ::close(_fd);
        auto err = errno;
        if (result == -1 && close_handlers.contains((SocketCloseError)err)) {
            close_handlers[(SocketCloseError)err]();
        }
    }
    void Socket::connect(const struct sockaddr* address, socklen_t address_len) {
        auto result = ::connect(_fd, address, address_len);
        auto err = errno;
        if (result == -1 && connect_handlers.contains((SocketConnectError)err)) {
            connect_handlers[(SocketConnectError)err]();
        }
    }
    void Socket::connect(const struct sockaddr_in* address) {connect((struct sockaddr*)address, sizeof(*address));}
    void Socket::connect(const struct sockaddr_in6* address) {connect((struct sockaddr*)address, sizeof(*address));}
    void Socket::create(SocketFamily family, Type type) {
        _fd = ::socket((int)family, (int)type, 0);
        auto err = errno;
        if (_fd == -1 && create_handlers.contains((SocketCreateError)err)) {
            create_handlers[(SocketCreateError)err]();
        }
    }
    void Socket::receive(void* buffer, size_t length, std::initializer_list<SocketReceiveFlags> flags) {
        int rflags = 0;
        for (auto f : flags) rflags | (int)f;
        auto result = ::recv(_fd, buffer, length, rflags);
        auto err = errno;
        if (result == -1 && receive_handlers.contains((SocketReceiveError)err)) {
            receive_handlers[(SocketReceiveError)err]();
        }
    }
    void Socket::send(const void* buf, size_t len, std::initializer_list<SocketSendFlags> flags = {}) {
        int sflags = 0;
        for (auto f : flags) sflags | (int)f;
        auto result = ::send(_fd, buf, len, sflags);
        auto err = errno;
        if (result == -1 && send_handlers.contains((SocketSendError)err)) {
            send_handlers[(SocketSendError)err]();
        }
    }
    Socket::Socket(): _auto_close(true) {}
    Socket::Socket(bool auto_close): _auto_close{auto_close} {}
    Socket::~Socket() {
        if (_auto_close) close();
    }

    bool Socket::validate(const std::string& input, LibSocket::Validator v) {
        switch (v) {
            case Validator::IPV4: return std::regex_match(input, ipv4_regex);
            case Validator::IPV6: return std::regex_match(input, ipv6_regex);
            case Validator::PORT: return std::regex_match(input, port_regex);
        }
    }
} // LibSocket