//
// Created by Robert Sale on 4/17/23.
//

#ifndef CLIENTSERVERCHATAPP_SOCKET_H
#define CLIENTSERVERCHATAPP_SOCKET_H

#include "Errors.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <functional>
#include <map>
#include <initializer_list>
#include <string>
#include <regex>

namespace LibSocket {
    enum class SocketFamily {
        /// Local communication
        UNIX = AF_UNIX,
        /// Synonym for AF_UNIX
        LOCAL = AF_LOCAL,
        /// IPv4 Internet protocols
        INET = AF_INET,
//        /// Amateur radio AX.25 protocol
//        AX25 = AF_AX25,
        /// IPX - Novell protocols
        IPX = AF_IPX,
        /// AppleTalk
        APPLETALK = AF_APPLETALK,
//        /// ITU-SizeType X.25 / ISO-8208 protocol
//        X25 = AF_X25,
        /// IPv6 Internet protocols
        INET6 = AF_INET6,
        /// DECet protocol sockets
        DECnet = AF_DECnet,
//        /// Key management protocol, originally developed for usage with IPsec
//        KEY = AF_KEY,
//        /// Kernel user interface device
//        NETLINK = AF_NETLINK,
//        /// Low-level packet interface
//        PACKET = AF_PACKET,
//        /// Reliable Datagram Sockets (RDS) protocol
//        RDS = AF_RDS,
//        /// Generic PPP transport layer, for setting up L2 tunnels (L2TP and PPPoE)
//        PPPOX = AF_PPPOX,
//        /// Logical link control (IEEE 802.2 LLC) protocol
//        LLC = AF_LLC,
//        /// InfiniBand native addressing
//        IB = AF_IB,
//        /// Multiprotocol Label Switching
//        MPLS = AF_MPLS,
//        /// Controller Area Network automotive bus protocol
//        CAN = AF_CAN,
//        /// TIPC, "cluster domain sockets" protocol
//        TIPC = AF_TIPC,
//        /// Bluetooth low-level socket protocol
//        BLUETOOTH = AF_BLUETOOTH,
//        /// Interface to kernel crypto API
//        ALG = AF_ALG,
        /// VSOCK (originally "VMWare VSockets") protocol for hypervisor-guest communication
        VSOCK = AF_VSOCK,
//        /// KCM (kernel connection multiplexer) interface
//        KCM = AF_KCM,
//        /// XDP (express data path) interface
//        XDP = AF_XDP,
    };
    enum class Type {
        /**
         *  @details Provides sequenced, reliable, two-way, connection-based
         *      byte streams.  An out-of-band data transmission mechanism
         *      may be supported.
         *  @example TCP
         */
        STREAM = SOCK_STREAM,
        /**
         *  @details Supports datagrams (connectionless, unreliable messages of
         *      a fixed maximum length).
         *  @example UDP
         */
        DATAGRAM = SOCK_DGRAM,
        /**
         * Provides a sequenced, reliable, two-way connection-based
         *    data transmission path for datagrams of fixed maximum
         *    length; a consumer is required to read an entire packet
         *    with each input system call.
         */
        SEQUENCED_PACKET = SOCK_SEQPACKET,
        /**
         * Provides raw network protocol access
         */
        RAW = SOCK_RAW,
        /**
         * Provides a reliable datagram layer that does not guarantee
         *    ordering.
         */
        RDM = SOCK_RDM
    };
    enum class IPType {
        V4 = (int)SocketFamily::INET,
        V6 = (int)SocketFamily::INET6
    };
    enum class SocketOptions {
        /**
         * Returns a value indicating whether or not this socket has
              been marked to accept connections with listen(2).  The
              value 0 indicates that this is not a listening socket, the
              value 1 indicates that this is a listening socket.  This
              socket option is read-only
         */
        ACCEPT_CONNECTIONS = SO_ACCEPTCONN,

    };
    enum class SocketSendFlags {
        /**
         * Don't use a gateway to send out the packet, send to hosts
              only on directly connected networks.  This is usually used
              only by diagnostic or routing programs.  This is defined
              only for protocol families that route; packet sockets
              don't.
         */
        DONT_ROUTE = MSG_DONTROUTE,
        /**
         * Enables nonblocking operation; if the operation would
              block, EAGAIN or EWOULDBLOCK is returned.  This provides
              similar behavior to setting the O_NONBLOCK flag (via the
              fcntl(2) F_SETFL operation), but differs in that
              MSG_DONTWAIT is a per-call option, whereas O_NONBLOCK is a
              setting on the open file description (see open(2)), which
              will affect all threads in the calling process and as well
              as other processes that hold file descriptors referring to
              the same open file description.
         */
        DONT_WAIT = MSG_DONTWAIT,
        /**
         * Terminates a record (when this notion is supported, as for
              sockets of type SOCK_SEQPACKET).
         */
        EOR = MSG_EOR,
        /**
         * Don't generate a SIGPIPE signal if the peer on a stream-
              oriented socket has closed the connection.  The EPIPE
              error is still returned.  This provides similar behavior
              to using sigaction(2) to ignore SIGPIPE, but, whereas
              MSG_NOSIGNAL is a per-call feature, ignoring SIGPIPE sets
              a process attribute that affects all threads in the
              process.
         */
        NO_SIGNAL = MSG_NOSIGNAL,
        /**
         * Sends out-of-band data on sockets that support this notion
              (e.g., of type SOCK_STREAM); the underlying protocol must
              also support out-of-band data.
         */
        OUT_OF_BAND = MSG_OOB
    };
    enum class SocketReceiveFlags {
        /**
         * Peeks at an incoming message. The data is
             treated as unread and the next recv() or
             similar function shall still return this
             data.
         */
        PEEK = MSG_PEEK,
        /**
         * Requests out-of-band data. The significance
             and semantics of out-of-band data are
             protocol-specific.
         */
        OUT_OF_BAND = MSG_OOB,
        /**
         * On SOCK_STREAM sockets this requests that
             the function block until the full amount of
             data can be returned. The function may
             return the smaller amount of data if the
             socket is a message-based socket, if a
             signal is caught, if the connection is
             terminated, if MSG_PEEK was specified, or
             if an error is pending for the socket.
         */
        WAIT_ALL = MSG_WAITALL,
        /**
         * Enables nonblocking operation; if the operation would block, the call fails
         * with the error EAGAIN or EWOULDBLOCK (this can also be enabled using the
         * O_NONBLOCK flag with the F_SETFL fcntl(2)).
         */
        DONT_WAIT = MSG_DONTWAIT
    };
    const extern std::regex ipv4_regex;
    const extern std::regex ipv6_regex;
    const extern std::regex port_regex;
    enum class Validator {
        IPV4 = (int)SocketFamily::INET,
        IPV6 = (int)SocketFamily::INET6,
        PORT
    };
    /**
     * C++ API for the BSD socket library
     * @details Every method has a error handler map. If you move lambdas
     * into the map they will be executed automatically. Makes code organization
     * a lot more intuitive.
     * @tparam SizeType integral used to store the size of the message
     */
    template<typename SizeType>
    class Socket {
        static_assert(std::is_integral<SizeType>(), "SizeType must be an integral data type");
    protected:
        using SocketFileDescriptor = int;
    public:
        /// File Descriptor for socket
        SocketFileDescriptor _fd{-1};
        /// If true, will close this socket in the destructor
        bool _auto_close;

        Socket(): _auto_close{true}, _fd{-1} {}
        explicit Socket(bool auto_close): _auto_close{auto_close}, _fd{-1} {}
        ~Socket() { if (_auto_close) close(); }

        /// A map of every possible close error and callback functions to run
        std::map<SocketCloseError, std::function<void()>> close_handlers;
        void close() {
            auto result = ::close(_fd);
            auto err = errno;
            if (result == -1 && close_handlers.contains((SocketCloseError)err)) {
                close_handlers[(SocketCloseError)err]();
            } else if (close_handlers.contains(SocketCloseError::SUCCESS)) {
                close_handlers[SocketCloseError::SUCCESS]();
            }
        }

        /// A map of every possible create error and callback functions to run
        std::map<SocketCreateError, std::function<void()>> create_handlers;
        /**
         * Create a new socket and set it internally
         * @param family The socket family used by the new socket (see LibSocket::SocketFamily for details)
         * @param type The type of socket (see LibSocket::Type for details)
         */
        void create(SocketFamily family, Type type) {
            _fd = ::socket((int)family, (int)type, 0);
            auto err = errno;
            if (_fd == -1 && create_handlers.contains((SocketCreateError)err)) {
                create_handlers[(SocketCreateError)err]();
            } else if (create_handlers.contains(SocketCreateError::SUCCESS)) {
                create_handlers[SocketCreateError::SUCCESS]();
            }
        }

        /// A map of every possible receive error and callback functions to run
        std::map<SocketReceiveError, std::function<void(const std::string&, Socket*)>> receive_handlers;
        /**
         * Receive a message from the socket
         * @param buffer where the message will be stored
         * @param length of the message
         * @param flags see LibSocket::SocketReceiveFlags for more info (defaults to empty list)
         */
        size_t receive(void* buffer, size_t length, std::initializer_list<SocketReceiveFlags> flags) {
            int rflags = 0;
            for (auto f : flags) rflags |= (int)f;
            auto result = ::recv(_fd, buffer, length, rflags);
            auto err = errno;
            if (result == -1 && receive_handlers.contains((SocketReceiveError)err)) {
                receive_handlers[(SocketReceiveError)err]("", this);
            } else if (result == 0 && receive_handlers.contains(SocketReceiveError::DISCONNECTING)) {
                receive_handlers[SocketReceiveError::DISCONNECTING]("", this);
            } else if (receive_handlers.contains(SocketReceiveError::SUCCESS)) {
                std::string message{(char*) buffer};
                receive_handlers[SocketReceiveError::SUCCESS](message, this);
            }
            return result;
        }
         /**
          * Receives a message
          * @tparam T integral used to determine the size of the message
          * @param flags see LibSocket::SocketReceiveFlags for more info (defaults to empty list)
          * @return string result of what was received
          */
        std::string receive_str(std::initializer_list<SocketReceiveFlags> flags) {
            SizeType size = 0;
            int rflags{0};
            for(auto f: flags) rflags |= (int)f;
            auto res = ::recv(_fd, &size, sizeof size, rflags);
            char buff[size + 1];
            memset(buff, 0, size + 1);
            this->receive(buff, size, flags);
            return {buff};
        }

        /// A map of every possible send error and callback functions to run
        std::map<SocketSendError, std::function<void(const std::string&)>> send_handlers;
        /**
         * Sends a message using the stored file descriptor
         * @param buf pointer to bytes
         * @param len length of payload in bytes
         * @param flags see LibSocket::SocketSendFlags for more info (Defaults to empty list)
         */
        void send(const void* buf, size_t len, std::initializer_list<SocketSendFlags> flags) {
            int sflags = 0;
            for (auto f : flags) sflags |= (int)f;
            auto result = ::send(_fd, buf, len, sflags);
            auto err = errno;
            std::string payload = (char*) buf;
            if (result == -1 && send_handlers.contains((SocketSendError)err)) {
                send_handlers[(SocketSendError)err](payload);
            } else if (send_handlers.contains(SocketSendError::SUCCESS)) {
                send_handlers[SocketSendError::SUCCESS](payload);
            }
        }

        /**
         * Sends a message in one fell swoop, prepending the message with the size
         * @param message
         * @param flags
         */
        void full_send(std::string message, std::initializer_list<SocketSendFlags> flags) {
            char buff[message.size() + sizeof(SizeType) + 1];
            memset(buff, 0, message.size() + sizeof(SizeType) + 1);
            SizeType size = (SizeType)message.size();

            memcpy(buff, &size, sizeof size);
            strcpy(buff+sizeof(SizeType), message.c_str());
            send(buff, message.size() + sizeof(SizeType) + 1, flags);
        }


        /**
         * Sends a message using a partial message loop. Dynamically sizes the first message based on SizeType.
         * Errors are automatically handled by send().
         * @tparam T integral for storing the size of the message
         * @param message string being sent
         * @param flags see SocketSendFlags for more information
         */
        void partial_send(std::string message, std::initializer_list<SocketSendFlags> flags) {
            SizeType size = (SizeType)message.size();
            send(&size, sizeof(SizeType), flags);
            send(message.c_str(), message.size(), flags);
        }

        /**
         * Sets own file descriptor to file descriptor set
         * @param fds the set
         */
        void set_to_fds(fd_set& fds) const { FD_SET(_fd, &fds); }

        /**
         * Gets the socket's file descriptor
         * @return file descriptor
         */
        const SocketFileDescriptor& get_fd() {return _fd;}

        /**
         * Checks if _fd is set on the file descriptor set
         * @param fds the file descriptor set
         * @return is _fd set on fds
         */
        bool fds_is_set(const fd_set& fds) {return FD_ISSET(_fd, &fds);}

        /**
         * Zeroes out all of the bits on the file descriptor set
         * @param fds the file descriptor set
         */
        void fds_zero(fd_set& fds) const { FD_ZERO(&fds); }

        /**
         * Sets _fd to the file descriptor set
         * @param fds the file descriptor set
         */
        void fds_set(fd_set& fds) const { FD_SET(_fd, &fds); }
        /**
         * Clears _fd from the file descriptor set
         * @param fds the file descriptor set
         */
        void fds_clr(fd_set& fds) const { FD_CLR(_fd, &fds); }

        /**
         * Validates an address-related string
         * @param input string to validate
         * @param v The validator to use
         * @return is valid
         */
        static bool validate(const std::string& input, Validator v) {
            switch (v) {
                case Validator::IPV4: return std::regex_match(input, ipv4_regex);
                case Validator::IPV6: return std::regex_match(input, ipv6_regex);
                case Validator::PORT: return std::regex_match(input, port_regex);
            }
        }
    };

    template<typename SizeType>
    class ServerSocket: public Socket<SizeType> {
        static_assert(std::is_integral<SizeType>(), "SizeType must be an integral data type");
    protected:
        using Socket = Socket<SizeType>;
        fd_set _read_fds;
        size_t _max_users;
    public:
        ServerSocket(size_t max_users, bool auto_close): Socket{auto_close}, _max_users{max_users}, _read_fds{} {}
        ServerSocket(): Socket{true}, _max_users{10}, _read_fds{} {}
        explicit ServerSocket(size_t max_users): Socket{true}, _max_users{max_users}, _read_fds{} {}
        explicit ServerSocket(bool auto_close): Socket{auto_close}, _max_users{10}, _read_fds{} {}
        /// A map of every possible accept error and callback functions to run
        std::map<SocketAcceptError, std::function<void(std::shared_ptr<Socket>)>> accept_handlers;
        /**
         * Extracts the first connection on the queue of pending connections and creates a socket
         * @param address optional address
         * @param len optional length of address
         */
        void accept(struct sockaddr* address, socklen_t* len) {
            auto result = ::accept(this->_fd, address, len);
            auto err = errno;
            if (result == -1 && accept_handlers.contains((SocketAcceptError)err)) {
                accept_handlers[(SocketAcceptError)err](nullptr);
                return;
            } else if (accept_handlers.contains(SocketAcceptError::SUCCESS)) {
                std::shared_ptr<Socket> client = std::make_shared<Socket>();
                client->_fd = result;
                accept_handlers[SocketAcceptError::SUCCESS](client);
            }
        }

        /// A map of every possible bind error and callback functions to run
        std::map<SocketBindError, std::function<void()>> bind_handlers;
        /**
         * Binds socket to a new listening address
         * @param address address to bind
         * @param address_len length of address
         */
        void bind(const struct sockaddr* address, socklen_t address_len) {
            auto result = ::bind(this->_fd, address, address_len);
            auto err = errno;
            if (result == -1 && bind_handlers.contains((SocketBindError)err)) {
                bind_handlers[(SocketBindError)err]();
            } else if (bind_handlers.contains(SocketBindError::SUCCESS)) {
                bind_handlers[SocketBindError::SUCCESS]();
            }
        }

        /**
         * Connects or binds socket conveniently using IPV4
         * @param ip IP of the endpoint
         * @param port port of the endpoint
         */
        void bind_v4(const std::string& ip, const std::string& port) {
            auto ip_validator = Validator::IPV4;
            if (!this->validate(ip, ip_validator) && bind_handlers.contains(SocketBindError::INVALID_ADDRESS)) {
                bind_handlers[SocketBindError::INVALID_ADDRESS]();
                return;
            }
            if (!this->validate(port, Validator::PORT) && bind_handlers.contains(SocketBindError::INVALID_PORT)) {
                bind_handlers[SocketBindError::INVALID_PORT]();
                return;
            }
            struct sockaddr_in out{};
            out.sin_family = (int)ip_validator;
            out.sin_port = htons(stoi(port));
            if (inet_pton((int)ip_validator, ip.c_str(), &out.sin_addr) <= 0 && bind_handlers.contains(SocketBindError::INVALID_ADDRESS)) {
                bind_handlers[SocketBindError::INVALID_ADDRESS]();
            }
            this->bind((struct sockaddr*)&out, sizeof out);
        }
        /**
         * Connects or binds socket conveniently using IPV4
         * @param ip IP of the endpoint
         * @param port port of the endpoint
         */
        void bind_v6(const std::string& ip, const std::string& port) {
            auto ip_validator = Validator::IPV6;
            if (!this->validate(ip, ip_validator) && bind_handlers.contains(SocketBindError::INVALID_ADDRESS)) {
                bind_handlers[SocketBindError::INVALID_ADDRESS]();
                return;
            }
            if (!this->validate(port, Validator::PORT) && bind_handlers.contains(SocketBindError::INVALID_PORT)) {
                bind_handlers[SocketBindError::INVALID_PORT]();
                return;
            }
            struct sockaddr_in6 out{};
            out.sin6_family = (int)ip_validator;
            out.sin6_port = htons(stoi(port));
            if (inet_pton((int)ip_validator, ip.c_str(), &out.sin6_addr) <= 0 && bind_handlers.contains(SocketBindError::INVALID_ADDRESS)) {
                bind_handlers[SocketBindError::INVALID_ADDRESS]();
            }
            this->bind((struct sockaddr*)&out, sizeof out);
        }
        /// A map of every possible listen error and callback functions to run
        std::map<SocketListenError, std::function<void()>> listen_handlers;
        void listen() {
            auto result = ::listen(this->_fd, _max_users);
            auto err = (SocketListenError)errno;
            if (result == -1 && listen_handlers.contains(err)) {
                listen_handlers[err]();
            } else if (listen_handlers.contains(SocketListenError::SUCCESS)) {
                listen_handlers[SocketListenError::SUCCESS]();
            }
        }

        void select(int max_sd, struct timeval* timeout) {
            auto result = ::select(max_sd + 1, &_read_fds, nullptr, nullptr, timeout);
            auto err = errno;
            if (result == -1 && select_handlers.contains((SocketSelectError)err)) {
                select_handlers[(SocketSelectError)err]();
            } else if (select_handlers.contains(SocketSelectError::SUCCESS)) {
                select_handlers[SocketSelectError::SUCCESS]();
            }
        }

        /// A map of every possible bind error and callback functions to run
        std::map<SocketSelectError, std::function<void()>> select_handlers;

    };
    template<typename SizeType>
    class ClientSocket: public Socket<SizeType> {
    protected:
        using Socket = Socket<SizeType>;
    public:
        ClientSocket(): Socket{true} {}
        explicit ClientSocket(bool auto_close): Socket{auto_close} {}
        /// A map of every possible connect error and callback functions to run
        std::map<SocketConnectError, std::function<void()>> connect_handlers;
        /**
         * Connects socket to address (connection-mode sockets only)
         * @param address the address struct
         * @param address_len length of the sockaddr structure pointed to by address
         */
        void connect(const struct sockaddr* address, socklen_t address_len) {
            auto result = ::connect(this->_fd, address, address_len);
            auto err = errno;
            if (result == -1 && connect_handlers.contains((SocketConnectError)err)) {
                connect_handlers[(SocketConnectError)err]();
            } else if (connect_handlers.contains(SocketConnectError::SUCCESS)) {
                connect_handlers[SocketConnectError::SUCCESS]();
            }
        }

        /**
         * Connects or binds socket conveniently using IPV4 or IPV6 syntax
         * @param ip IP of the endpoint
         * @param port port of the endpoint
         * @param out the resulting socket address
         */
        void connect_v4(const std::string& ip, const std::string& port) {
            auto ip_validator = Validator::IPV4;
            if (!this->validate(ip, ip_validator) && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                connect_handlers[SocketConnectError::INVALID_ADDRESS]();
                return;
            }
            if (!this->validate(port, Validator::PORT) && connect_handlers.contains(SocketConnectError::INVALID_PORT)) {
                connect_handlers[SocketConnectError::INVALID_PORT]();
                return;
            }
            struct sockaddr_in out{};
            out.sin_family = (int)ip_validator;
            out.sin_port = htons(stoi(port));
            if (inet_pton((int)ip_validator, ip.c_str(), &out.sin_addr) <= 0 && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                connect_handlers[SocketConnectError::INVALID_ADDRESS]();
            }
            connect((struct sockaddr*)&out, sizeof out);
        }
        /**
         * Connects or binds socket conveniently using IPV4 or IPV6 syntax
         * @param ip IP of the endpoint
         * @param port port of the endpoint
         * @param out the resulting socket address
         */
        void connect_v6(const std::string& ip, const std::string& port) {
            auto ip_validator = Validator::IPV6;
            if (!this->validate(ip, ip_validator) && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                connect_handlers[SocketConnectError::INVALID_ADDRESS]();
                return;
            }
            if (!this->validate(port, Validator::PORT) && connect_handlers.contains(SocketConnectError::INVALID_PORT)) {
                connect_handlers[SocketConnectError::INVALID_PORT]();
                return;
            }
            struct sockaddr_in6 out{};
            out.sin6_family = (int)ip_validator;
            out.sin6_port = htons(stoi(port));
            if (inet_pton((int)ip_validator, ip.c_str(), &out.sin6_addr) <= 0 && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                connect_handlers[SocketConnectError::INVALID_ADDRESS]();
            }
            connect((struct sockaddr*)&out, sizeof out);
        }
    };

} // LibSocket

#endif //CLIENTSERVERCHATAPP_SOCKET_H
