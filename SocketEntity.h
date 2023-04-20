//
// Created by Robert Sale on 4/9/23.
//

#ifndef CLIENTSERVERCHATAPP_SOCKETENTITY_H
#define CLIENTSERVERCHATAPP_SOCKETENTITY_H

#include "Console.h"

namespace ClientServerChatApp {
    /**
     * The various signals which might occur using the Socket
     */
    enum class SocketSignal {
        SUCCESS, SHUTDOWN, DISCONNECT, BIND_ERROR, CONNECT_ERROR, SETUP_ERROR,
        STARTUP_ERROR, ADDRESS_ERROR, PARAMETER_ERROR, MESSAGE_ERROR, CONTINUE,
        SELECT_ERROR, ACCEPT_ERROR, LISTEN_ERROR
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

        SmartConsole::Console* console;

        /// Turns response code into developer readable signal
        static SocketSignal resolve_response(SmartConsole::Console* console, ssize_t _response_code);
    public:
        /**
         * The Socket established by the client (int for file descriptor)
         */
        SocketFileDescriptor io;
        /// This gets set after each Socket message
        std::atomic<SocketSignal> active_signal;

        SocketEntity();
        ~SocketEntity();
        /**
         * Checks file descriptor and returns whether the Socket was created successfully or not
         * @return
         */
        [[nodiscard]] bool socket_created() const;
        /**
         * Manually create Socket.
         */
        SocketSignal create_socket();
    private:
    };
} // ClientServerChatApp

#endif //CLIENTSERVERCHATAPP_SOCKETENTITY_H
