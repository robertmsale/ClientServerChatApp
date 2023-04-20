//
// Created by Robert Sale on 4/17/23.
//

#ifndef CLIENTSERVERCHATAPP_ERRORS_H
#define CLIENTSERVERCHATAPP_ERRORS_H

#include <cerrno>
#include <initializer_list>

namespace LibSocket {

    enum class SocketCreateError {
        /**
         * Permission to create a socket of the specified type and/or
         *    protocol is denied.
         */
        ACCESS_DENIED = EACCES,
        /**
         * The implementation does not support the specified address
         *    family.
         */
        ADDRESS_FAMILY_NOT_SUPPORTED = EAFNOSUPPORT,
        /// Unknown protocol, invalid flags in *type*, or protocol family not available.
        INVALID_FLAG = EINVAL,
        /// Per-process or system-wide limit on the total number of open files has been reached
        OPEN_FD_LIMIT = ENFILE,
        /// Insufficient memory available.
        NO_BUFFERS_AVAILABLE = ENOBUFS,
        /// Insufficient memory available.
        NO_MEMORY_AVAILABLE = ENOMEM,
        /// The protocol type or the specified protocol is not supported within this domain
        PROTOCOL_NOT_SUPPORTED = EPROTONOSUPPORT,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    /// A list of all create errors, not including success, iterable
    constexpr std::initializer_list<SocketCreateError> all_create_errors = {
            SocketCreateError::ACCESS_DENIED,
            SocketCreateError::ADDRESS_FAMILY_NOT_SUPPORTED,
            SocketCreateError::INVALID_FLAG,
            SocketCreateError::OPEN_FD_LIMIT,
            SocketCreateError::NO_BUFFERS_AVAILABLE,
            SocketCreateError::NO_MEMORY_AVAILABLE,
            SocketCreateError::PROTOCOL_NOT_SUPPORTED
    };
    enum class SocketAcceptError {
        /// The socket is nonblocking and no connections are present to be accepted
        AGAIN = EAGAIN,
        /// The socket is nonblocking and no connections are present to be accepted
        WOULD_BLOCK = EWOULDBLOCK,
        /// sockfd is not an open file descriptor
        BAD_FD = EBADF,
        /// Connection was aborted
        ABORTED = ECONNABORTED,
        /// addr is not in a writable part of the user address space
        ADDR_NOT_WRITEABLE = EFAULT,
        /// System call was interrupted by a signal that was caught before a valid connection arrived
        INTERRUPTED = EINTR,
        /// Socket is not listening, invalid flags, or addrlen is invalid
        INVALID = EINVAL,
        /// Per-process or system-wide limit on the total number of open files has been reached
        OPEN_FD_LIMIT = ENFILE,
        /// Insufficient memory available.
        NO_BUFFERS_AVAILABLE = ENOBUFS,
        /// Insufficient memory available.
        NO_MEMORY_AVAILABLE = ENOMEM,
        /// File descriptor does not refer to a socket
        NOT_A_SOCKET = ENOTSOCK,
        /// The socket is not a `SOCK_STREAM` or `Type::STREAM`
        NOT_A_STREAM = EOPNOTSUPP,
        /// Firewall rules forbid connection
        FIREWALL_BLOCKED = EPERM,
        /// Protocol error
        PROTOCOL = EPROTO,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    /// A list of all accept errors, not including success, iterable
    constexpr std::initializer_list<SocketAcceptError> all_accept_errors = {
        SocketAcceptError::AGAIN,
        SocketAcceptError::WOULD_BLOCK,
        SocketAcceptError::BAD_FD,
        SocketAcceptError::ABORTED,
        SocketAcceptError::ADDR_NOT_WRITEABLE,
        SocketAcceptError::INTERRUPTED,
        SocketAcceptError::INVALID,
        SocketAcceptError::OPEN_FD_LIMIT,
        SocketAcceptError::NO_BUFFERS_AVAILABLE,
        SocketAcceptError::NO_MEMORY_AVAILABLE,
        SocketAcceptError::NOT_A_SOCKET,
        SocketAcceptError::NOT_A_STREAM,
        SocketAcceptError::FIREWALL_BLOCKED,
        SocketAcceptError::PROTOCOL,
    };
    enum class SocketBindError {
        /// Address is protected and the user is not the superuser
        /// or search permission is denied on a component
        /// of the path prefix.
        ACCESS_DENIED = EACCES,
        /**
         * The given address is currently in use, or the port number was specified as zero
         * but upon attempting to bind to an ephemeral port, it was determined that all port
         * numbers in the ephemeral port range are currently in use
         */
        ADDRESS_IN_USE = EADDRINUSE,
        /// sockfd is not an open file descriptor
        BAD_FD = EBADF,
        /// Socket is already bound to an address, invalid flags, or addr is invalid
        INVALID = EINVAL,
        /// File descriptor does not refer to a socket
        NOT_A_SOCKET = ENOTSOCK,
        /// A nonexistent interface was requested or the requested address was not local
        ADDRESS_NOT_AVAILABLE = EADDRNOTAVAIL,
        /// addr points outside the user's accessible address space
        OUTSIDE_ADDRESS_SPACE = EFAULT,
        /// Too many symbolic links were encountered in resolving addr
        EXCEEDED_SYMLINK_RECURSION_DEPTH = ELOOP,
        /// addr is too long
        NAME_TOO_LONG = ENAMETOOLONG,
        /// A component in the directory prefix of the socket pathname does not exist
        DOESNT_EXIST = ENOENT,
        /// Insufficient kernel memory was available
        NO_MEMORY_AVAILABLE = ENOMEM,
        /// A component of the path prefix is not a directory
        NOT_A_DIRECTORY = ENOTDIR,
        /// Socket inode resides in read-only filesystem
        READ_ONLY = EROFS,
        /// Address string did not pass validation
        INVALID_ADDRESS,
        /// Port string did not pass validation
        INVALID_PORT,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    /// A list of all bind errors, not including success, iterable
    constexpr std::initializer_list<SocketBindError> all_bind_errors = {
        SocketBindError::ACCESS_DENIED,
        SocketBindError::ADDRESS_IN_USE,
        SocketBindError::BAD_FD,
        SocketBindError::INVALID,
        SocketBindError::NOT_A_SOCKET,
        SocketBindError::ADDRESS_NOT_AVAILABLE,
        SocketBindError::OUTSIDE_ADDRESS_SPACE,
        SocketBindError::EXCEEDED_SYMLINK_RECURSION_DEPTH,
        SocketBindError::NAME_TOO_LONG,
        SocketBindError::DOESNT_EXIST,
        SocketBindError::NO_MEMORY_AVAILABLE,
        SocketBindError::NOT_A_DIRECTORY,
        SocketBindError::READ_ONLY,
        SocketBindError::INVALID_ADDRESS,
        SocketBindError::INVALID_PORT,
    };
    enum class SocketCloseError {
        /// Not a valid file descriptor
        BAD_FD = EBADF,
        /// call was interrupted by a signal
        INTERRUPTED = EINTR,
        /// An I/O error occurred
        IO = EIO,
        /// Exceeds available storage space
        NO_SPACE = ENOSPC,
        /// Exceeds write quota
        QUOTA = EDQUOT,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    /// A list of all close errors, not including success, iterable
    constexpr std::initializer_list<SocketCloseError> all_close_errors = {
        SocketCloseError::BAD_FD,
        SocketCloseError::INTERRUPTED,
        SocketCloseError::IO,
        SocketCloseError::NO_SPACE,
        SocketCloseError::QUOTA,
    };
    enum class SocketConnectError {
        /// For UNIX domain sockets, which are identified by pathname:
        /// Write permission is denied on the socket file, or search
        /// permission is denied for one of the directories in the
        /// path prefix.
        ACCESS_DENIED = EACCES,
        /// The user tried to connect to a broadcast address without
        /// having the socket broadcast flag enabled or the connection
        /// request failed because of a local firewall rule.
        FIREWALL_BLOCKED = EPERM,
        /// Local address is already in use.
        ADDRESS_IN_USE = EADDRINUSE,
        /// (Internet domain sockets) The socket referred to by sockfd
        /// had not previously been bound to an address and, upon
        /// attempting to bind it to an ephemeral port, it was
        /// determined that all port numbers in the ephemeral port
        /// range are currently in use.
        ADDRESS_NOT_AVAILABLE = EADDRNOTAVAIL,
        /// The passed address didn't have the correct address family
        /// in its sa_family field.
        FAMILY_NOT_SUPPORTED = EAFNOSUPPORT,
        /// For nonblocking UNIX domain sockets, the socket is
        /// nonblocking, and the connection cannot be completed
        /// immediately.  For other socket families, there are
        /// insufficient entries in the routing cache.
        NOT_RIGHT_NOW = EAGAIN,
        /// The socket is nonblocking and a previous connection
        /// attempt has not yet been completed.
        PREVIOUS_NOT_DONE = EALREADY,
        /// sockfd is not a valid open file descriptor.
        BAD_FD = EBADF,
        /// A connect() on a stream socket found no one listening on
        /// the remote address.
        CONNECTION_REFUSED = ECONNREFUSED,
        /// The socket structure address is outside the user's address space.
        FAULT = EFAULT,
        /// The socket is nonblocking and the connection cannot be
        /// completed immediately.  (UNIX domain sockets failed with
        /// EAGAIN instead.)  It is possible to select(2) or poll(2)
        /// for completion by selecting the socket for writing.  After
        /// select(2) indicates writability, use getsockopt(2) to read
        /// the SO_ERROR option at level SOL_SOCKET to determine
        /// whether connect() completed successfully (SO_ERROR is
        /// zero) or unsuccessfully (SO_ERROR is one of the usual
        /// error codes listed here, explaining the reason for the
        /// failure).
        IN_PROGRESS = EINPROGRESS,
        ///The system call was interrupted by a signal that was caught
        INTERRUPTED = EINTR,
        /// The socket is already connected.
        ALREADY_CONNECTED = EISCONN,
        /// Network is unreachable.
        UNREACHABLE = ENETUNREACH,
        /// The file descriptor sockfd does not refer to a socket.
        NOT_A_SOCKET = ENOTSOCK,
        /// The socket type does not support the requested
        /// communications protocol.  This error can occur, for
        /// example, on an attempt to connect a UNIX domain datagram
        /// socket to a stream socket.
        PROTOTYPE_NOT_SUPPORTED = EPROTOTYPE,
        /// Timeout while attempting connection.  The server may be
        /// too busy to accept new connections.  Note that for IP
        /// sockets the timeout may be very long when syncookies are
        /// enabled on the server.
        TIMED_OUT = ETIMEDOUT,
        /// Address string did not pass validation
        INVALID_ADDRESS,
        /// Port string did not pass validation
        INVALID_PORT,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketConnectError> all_connect_errors = {
        SocketConnectError::ACCESS_DENIED,
        SocketConnectError::FIREWALL_BLOCKED,
        SocketConnectError::ADDRESS_IN_USE,
        SocketConnectError::ADDRESS_NOT_AVAILABLE,
        SocketConnectError::FAMILY_NOT_SUPPORTED,
        SocketConnectError::NOT_RIGHT_NOW,
        SocketConnectError::PREVIOUS_NOT_DONE,
        SocketConnectError::BAD_FD,
        SocketConnectError::CONNECTION_REFUSED,
        SocketConnectError::FAULT,
        SocketConnectError::IN_PROGRESS,
        SocketConnectError::INTERRUPTED,
        SocketConnectError::ALREADY_CONNECTED,
        SocketConnectError::UNREACHABLE,
        SocketConnectError::NOT_A_SOCKET,
        SocketConnectError::PROTOTYPE_NOT_SUPPORTED,
        SocketConnectError::TIMED_OUT,
        SocketConnectError::INVALID_ADDRESS,
        SocketConnectError::INVALID_PORT,
    };
    enum class SocketGetPeerNameError {
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /// The addr argument points to memory not in a valid part of the process address space.
        FAULT = EFAULT,
        /// addrlen is invalid (e.g., is negative).
        INVALID = EINVAL ,
        /// Insufficient resources were available in the system to perform the operation.
        NO_RESOURCES = ENOBUFS,
        /// The socket is not connected.
        NOT_CONNECTED = ENOTCONN,
        /// The file descriptor sockfd does not refer to a socket.
        NOT_A_SOCKET = ENOTSOCK,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };

    constexpr std::initializer_list<SocketGetPeerNameError> all_get_peer_name_errors = {
        SocketGetPeerNameError::BAD_FD,
        SocketGetPeerNameError::FAULT,
        SocketGetPeerNameError::INVALID,
        SocketGetPeerNameError::NO_RESOURCES,
        SocketGetPeerNameError::NOT_CONNECTED,
        SocketGetPeerNameError::NOT_A_SOCKET,
    };
    enum class SocketGetSockNameError {
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /// The addr argument points to memory not in a valid part of the process address space.
        FAULT = EFAULT,
        /// addrlen is invalid (e.g., is negative).
        INVALID = EINVAL ,
        /// Insufficient resources were available in the system to perform the operation.
        NO_RESOURCES = ENOBUFS,
        /// The file descriptor sockfd does not refer to a socket.
        NOT_A_SOCKET = ENOTSOCK,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketGetSockNameError> all_get_sock_name_errors = {
        SocketGetSockNameError::BAD_FD,
        SocketGetSockNameError::FAULT,
        SocketGetSockNameError::INVALID,
        SocketGetSockNameError::NO_RESOURCES,
        SocketGetSockNameError::NOT_A_SOCKET,
    };
    enum class SocketGetSockOptError {
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /// The address pointed to by optval is not in a valid part of
        /// the process address space.  For getsockopt(), this error
        /// may also be returned if optlen is not in a valid part of
        /// the process address space.
        FAULT = EFAULT,
        /// optlen invalid in setsockopt().  In some cases this error
        /// can also occur for an invalid value in optval
        INVALID = EINVAL,
        /// The option is unknown at the level indicated
        OPTION_UNKNOWN = ENOPROTOOPT,
        /// The file descriptor sockfd does not refer to a socket
        NOT_A_SOCKET = ENOTSOCK,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketGetSockOptError> all_get_sock_opt_errors = {
        SocketGetSockOptError::BAD_FD,
        SocketGetSockOptError::FAULT,
        SocketGetSockOptError::INVALID,
        SocketGetSockOptError::OPTION_UNKNOWN,
        SocketGetSockOptError::NOT_A_SOCKET,
    };
    enum class SocketListenError {
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /**
         * The given address is currently in use, or the port number was specified as zero
         * but upon attempting to bind to an ephemeral port, it was determined that all port
         * numbers in the ephemeral port range are currently in use
         */
        ADDRESS_IN_USE = EADDRINUSE,
        /// The file descriptor sockfd does not refer to a socket..
        NOT_A_SOCKET = ENOTSOCK,
        /// The socket is not of a type that supports the listen() operation
        UNSUPPORTED_OPERATION = EOPNOTSUPP,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketListenError> all_listen_errors = {
        SocketListenError::BAD_FD,
        SocketListenError::ADDRESS_IN_USE,
        SocketListenError::NOT_A_SOCKET,
        SocketListenError::UNSUPPORTED_OPERATION,
    };
    enum class SocketReceiveError {
        /// The socket is nonblocking and no connections are present to be accepted
        AGAIN = EAGAIN,
        /// The socket is nonblocking and no connections are present to be accepted
        WOULD_BLOCK = EWOULDBLOCK,
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /// A remote host refused to allow the network connection
        /// (typically because it is not running the requested service).
        CONNECTION_REFUSED = ECONNREFUSED,
        /// The receive buffer pointer(s) point outside the process' address space
        FAULT = EFAULT,
        /// The receive was interrupted by delivery of a signal before any data was available
        INTERRUPTED = EINTR,
        /// Invalid argument passed
        INVALID_ARG = EINVAL,
        /// Could not allocate memory for recvmsg()
        NO_MEMORY = ENOMEM,
        /// The socket associated with a connection-oriented protocol has not been connected
        NOT_CONNECTED = ENOTCONN,
        /// The file descriptor sockfd does not refer to a socket
        NOT_A_SOCKET = ENOTSOCK,
        /// Socket is disconnecting
        DISCONNECTING,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketReceiveError> all_receive_errors = {
        SocketReceiveError::AGAIN,
        SocketReceiveError::WOULD_BLOCK,
        SocketReceiveError::BAD_FD,
        SocketReceiveError::CONNECTION_REFUSED,
        SocketReceiveError::FAULT,
        SocketReceiveError::INTERRUPTED,
        SocketReceiveError::INVALID_ARG,
        SocketReceiveError::NO_MEMORY,
        SocketReceiveError::NOT_CONNECTED,
        SocketReceiveError::NOT_A_SOCKET,
    };
    enum class SocketSelectError {
        /// The argument sockfd is not a valid file descriptor
        BAD_FD = EBADF,
        /// A signal was caught
        INTERRUPTED = EINTR,
        /// nfds is negative or exceeds the RLIMIT_NOFILE resource limit
        /// or value contained within timeout is invalid
        INVALID = EINVAL,
        /// Unable to allocate memory for internal tables
        NO_MEMORY = ENOMEM,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketSelectError> all_select_errors = {
        SocketSelectError::BAD_FD,
        SocketSelectError::INTERRUPTED,
        SocketSelectError::INVALID,
        SocketSelectError::NO_MEMORY,
    };
    enum class SocketSendError {
        /// The socket is nonblocking and no connections are present to be accepted
        AGAIN = EAGAIN,
        /// The socket is nonblocking and no connections are present to be accepted
        WOULD_BLOCK = EWOULDBLOCK,
        /// Write permission denied on the destination socket file
        ACCESS_DENIED = EACCES,
        /// Another fast open in progress
        FAST_OPEN_IN_PROGRESS = EALREADY,
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /// Connection reset by peer
        CONNECTION_RESET = ECONNRESET,
        /// The socket is not connection-mode, and no peer address is set
        DESTINATION_ADDRESS_REQUIRED = EDESTADDRREQ,
        /// Invalid user space address was specified
        FAULT = EFAULT,
        /// A signal occurred before any data was transmitted
        INTERRUPTED = EINTR,
        /// Invalid argument passed
        INVALID = EINVAL,
        /// The connection-mode socket was connected already but a
        /// recipient was specified.  (Now either this error is
        /// returned, or the recipient specification is ignored.)
        ALREADY_CONNECTED = EISCONN,
        /// The socket type requires that message be sent atomically,
        /// and the size of the message to be sent made this impossible.
        MESSAGE_SIZE_BAD = EMSGSIZE,
        /// The output queue for a network interface was full.  This
        /// generally indicates that the interface has stopped
        /// sending, but may be caused by transient congestion.
        /// (Normally, this does not occur in Linux.  Packets are just
        /// silently dropped when a device queue overflows.)
        NO_BUFFERS_AVAILABLE = ENOBUFS,
        /// No memory available
        NO_MEMORY = ENOMEM,
        /// Not connected and no target has been given
        NOT_CONNECTED = ENOTCONN,
        /// The file descriptor sockfd does not refer to a socket
        NOT_A_SOCKET = ENOTSOCK,
        /// Some bit in the flags argument is inappropriate for the socket type
        OPTION_NOT_SUPPORTED = EOPNOTSUPP,
        /// The local end has been shut down on a connection oriented socket.
        PIPE = EPIPE,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketSendError> all_send_errors = {
        SocketSendError::AGAIN,
        SocketSendError::WOULD_BLOCK,
        SocketSendError::ACCESS_DENIED,
        SocketSendError::FAST_OPEN_IN_PROGRESS,
        SocketSendError::BAD_FD,
        SocketSendError::CONNECTION_RESET,
        SocketSendError::DESTINATION_ADDRESS_REQUIRED,
        SocketSendError::FAULT,
        SocketSendError::INTERRUPTED,
        SocketSendError::INVALID,
        SocketSendError::ALREADY_CONNECTED,
        SocketSendError::MESSAGE_SIZE_BAD,
        SocketSendError::NO_BUFFERS_AVAILABLE,
        SocketSendError::NO_MEMORY,
        SocketSendError::NOT_CONNECTED,
        SocketSendError::NOT_A_SOCKET,
        SocketSendError::OPTION_NOT_SUPPORTED,
        SocketSendError::PIPE,
    };
    enum class SocketShutdownError {
        /// The argument sockfd is not a valid file descriptor.
        BAD_FD = EBADF,
        /// Invalid argument passed
        INVALID = EINVAL,
        /// Not connected and no target has been given
        NOT_CONNECTED = ENOTCONN,
        /// The file descriptor sockfd does not refer to a socket
        NOT_A_SOCKET = ENOTSOCK,
        /// Indicates successful creation and used by creation handler
        SUCCESS
    };
    constexpr std::initializer_list<SocketShutdownError> all_shutdown_errors = {
        SocketShutdownError::BAD_FD,
        SocketShutdownError::INVALID,
        SocketShutdownError::NOT_CONNECTED,
        SocketShutdownError::NOT_A_SOCKET,
    };
}
#endif //CLIENTSERVERCHATAPP_ERRORS_H
