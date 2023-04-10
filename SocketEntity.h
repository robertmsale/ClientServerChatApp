//
// Created by Robert Sale on 4/9/23.
//

#ifndef CLIENTSERVERCHATAPP_SOCKETENTITY_H
#define CLIENTSERVERCHATAPP_SOCKETENTITY_H

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

namespace ClientServerChatApp {
    /**
     * The various signals which might occur using the socket
     */
    enum class SocketSignal {
        SUCCESS, SHUTDOWN, DISCONNECT, BIND_ERROR, CONNECT_ERROR, SETUP_ERROR,
        STARTUP_ERROR, ADDRESS_ERROR, PARAMETER_ERROR, MESSAGE_ERROR, CONTINUE
    };
    /**
      * For receiving messages
      */
    struct ReceiveMessage {
        std::string message;
        SocketSignal code;
    };
    /**
     * This class is a base class used by the Client and Server and contains the logic shared by both
     * derived classes.
     */
    class SocketEntity {
    protected:
        /**
         * The Socket established by the client (int for file descriptor)
         */
        SocketFileDescriptor io;
        static SocketSignal resolve_response(ssize_t _response_code);
    public:
        SocketEntity();
        /**
         * Checks file descriptor and returns whether the socket was created successfully or not
         * @return
         */
        [[nodiscard]] bool socket_created() const;
        /**
         * Manually create socket.
         */
        SocketSignal create_socket();
    private:
    };
} // ClientServerChatApp

#endif //CLIENTSERVERCHATAPP_SOCKETENTITY_H
