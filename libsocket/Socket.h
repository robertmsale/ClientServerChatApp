//
// Created by Robert Sale on 4/17/23.
//

#ifndef CLIENTSERVERCHATAPP_SOCKET_H
#define CLIENTSERVERCHATAPP_SOCKET_H

#include "Errors.h"
#include "Address.h"

namespace LibSocket {
    using SocketFileDescriptor = int;
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
//        /// ITU-T X.25 / ISO-8208 protocol
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
        WAIT_ALL = MSG_WAITALL
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
     */
    class Socket {
        /// File Descriptor for socket
        SocketFileDescriptor _fd{-1};
        /// If true, will close this socket in the destructor
        bool _auto_close;
    public:
        /// A map of every possible accept error and callback functions to run
        std::map<SocketAcceptError, std::function<void()>> accept_handlers;
        /**
         * Extracts the first connection on the queue of pending connections and creates a socket
         * @param address optional address
         * @param len optional length of address
         */
        void accept(struct sockaddr* address, socklen_t* len);
        void accept();

        /// A map of every possible bind error and callback functions to run
        std::map<SocketBindError, std::function<void()>> bind_handlers;
        /**
         *
         * @param address
         * @param address_len
         */
        void bind(const struct sockaddr* address, socklen_t address_len);
        void bind(const struct sockaddr_in* address);
        void bind(const struct sockaddr_in6* address);

        /// A map of every possible connect error and callback functions to run
        std::map<SocketConnectError, std::function<void()>> connect_handlers;
        /**
         * Connects socket to address (connection-mode sockets only)
         * @param address the address struct
         * @param address_len length of the sockaddr structure pointed to by address
         */
        void connect(const struct sockaddr* address, socklen_t address_len);
        void connect(const struct sockaddr_in* address);
        void connect(const struct sockaddr_in6* address);
        /**
         * Connects or binds socket conveniently using IPV4 or IPV6 syntax
         * @param ip IP of the endpoint
         * @param port port of the endpoint
         * @param t IPType::V4 or IPType::V6
         * @param out the resulting socket address
         * @tparam T the sockaddr derived object
         */
        template<typename T>
        void connect_bind_resolve(const std::string& ip, const std::string& port, IPType t, std::unique_ptr<T>& out) {
            static_assert(std::is_base_of<T, struct sockaddr_in>::value || std::is_base_of<T, sockaddr_in6>::value,
                    "T must be of type sockaddr_in or sockaddr_in6");
            auto ip_validator = (Validator)t;
            if (!validate(ip, ip_validator) && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                connect_handlers[SocketConnectError::INVALID_ADDRESS]();
                return;
            }
            if (!validate(port, Validator::PORT) && connect_handlers.contains(SocketConnectError::INVALID_PORT)) {
                connect_handlers[SocketConnectError::INVALID_PORT]();
                return;
            }
            out = T{};
            if (std::is_base_of<T, struct sockaddr_in>::value) {
                out->sin_family = (int)t;
                out->sin_port = htons(stoi(port));
                if (inet_pton((int)t, ip.c_str(), &out->sin_addr) <= 0 && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                    connect_handlers[SocketConnectError::INVALID_ADDRESS]();
                }
            } else {
                out->sin6_family = (int)t;
                out->sin6_port = htons(stoi(port));
                if (inet_pton((int)t, ip.c_str(), &out->sin6_addr) <= 0 && connect_handlers.contains(SocketConnectError::INVALID_ADDRESS)) {
                    connect_handlers[SocketConnectError::INVALID_ADDRESS]();
                }
            }
        }

        /// A map of every possible close error and callback functions to run
        std::map<SocketCloseError, std::function<void()>> close_handlers;
        void close();

        /// A map of every possible create error and callback functions to run
        std::map<SocketCreateError, std::function<void()>> create_handlers;
        /**
         * Create a new socket and set it internally
         * @param family The socket family used by the new socket (see LibSocket::SocketFamily for details)
         * @param type The type of socket (see LibSocket::Type for details)
         */
        void create(SocketFamily family, Type type);

        /// A map of every possible receive error and callback functions to run
        std::map<SocketReceiveError, std::function<void()>> receive_handlers;
        /**
         * Receive a message from the socket
         * @param buffer where the message will be stored
         * @param length of the message
         * @param flags see LibSocket::SocketReceiveFlags for more info (defaults to empty list)
         */
        void receive(void* buffer, size_t length, std::initializer_list<SocketReceiveFlags> flags);
         /**
          * Receives a message
          * @tparam T integral used to determine the size of the message
          * @param flags see LibSocket::SocketReceiveFlags for more info (defaults to empty list)
          * @return string result of what was received
          */
        template<typename T,
                typename std::enable_if<std::is_integral<T>::value>
                >
        std::string receive(std::initializer_list<SocketSendFlags> flags) {
            T size = 0;
            receive(&size, sizeof size, flags);
            char buff[size + 1];
            memset(buff, 0, size + 1);
            receive(buff, size, flags);
            return {buff};
        }

        /// A map of every possible send error and callback functions to run
        std::map<SocketSendError, std::function<void()>> send_handlers;
        /**
         * Sends a message using the stored file descriptor
         * @param buf pointer to bytes
         * @param len length of payload in bytes
         * @param flags see LibSocket::SocketSendFlags for more info (Defaults to empty list)
         */
        void send(const void* buf, size_t len, std::initializer_list<SocketSendFlags> flags);

        /**
         * Sends a message in one fell swoop, prepending the message with the size
         * @tparam T integral used to store the size of the message
         * @param message
         * @param flags
         */
        template<typename T,
                typename std::enable_if<std::is_integral<T>::value>
                >
        void full_send(std::string message, std::initializer_list<SocketSendFlags> flags) {
            char buff[message.size()+sizeof(T)+1];
            memset(buff, 0, message.size()+sizeof(T)+1);
            T size = (T)message.size();

            memcpy(buff, &size, sizeof size);
            strcpy(buff, message.c_str());
            send(buff, message.size()+sizeof(T)+1, flags);
        }


        /**
         * Sends a message using a partial message loop. Dynamically sizes the first message based on T.
         * Errors are automatically handled by send().
         * @tparam T integral for storing the size of the message
         * @param message string being sent
         * @param flags see SocketSendFlags for more information
         */
        template<typename T,
                typename std::enable_if<std::is_integral<T>::value>
                >
        void partial_send(std::string message, std::initializer_list<SocketSendFlags> flags) {
            T size = (T)message.size();
            send(&size, sizeof(T), flags);
            send(message.c_str(), message.size(), flags);
        }

        Socket();
        explicit Socket(bool auto_close);
        ~Socket();

        /**
         * Validates an address-related string
         * @param input string to validate
         * @param v The validator to use
         * @return is valid
         */
        static bool validate(const std::string& input, Validator v);
    };

} // LibSocket

#endif //CLIENTSERVERCHATAPP_SOCKET_H
