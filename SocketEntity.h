//
// Created by Robert Sale on 4/9/23.
//

#ifndef CLIENTSERVERCHATAPP_SOCKETENTITY_H
#define CLIENTSERVERCHATAPP_SOCKETENTITY_H

#include "Console.h"

namespace ClientServerChatApp {
    /**
     * The various signals which might occur using the socket
     */
    enum class SocketSignal {
        SUCCESS, SHUTDOWN, DISCONNECT, BIND_ERROR, CONNECT_ERROR, SETUP_ERROR,
        STARTUP_ERROR, ADDRESS_ERROR, PARAMETER_ERROR, MESSAGE_ERROR, CONTINUE,
        SELECT_ERROR, ACCEPT_ERROR, LISTEN_ERROR
    };
    using MessageSignalType = std::string;
    namespace MessageSignals {
        constexpr MessageSignalType SRV_FULL() {return "SRV_FULL";}
    }
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
        /// This gets set after each socket message
        std::atomic<SocketSignal> active_signal;

        /// Child thread sends socket created successful, main receives it
        SyncPoint<bool> sync_socket_created;
        /// Child thread sends socket connection successful, main receives it
        SyncPoint<bool> sync_socket_established;

        SocketEntity();
        ~SocketEntity();
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
