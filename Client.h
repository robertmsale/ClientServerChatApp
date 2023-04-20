//
// Created by Robert Sale on 4/8/23.
//

#ifndef CLIENTSERVERCHATAPP_CLIENT_H
#define CLIENTSERVERCHATAPP_CLIENT_H

#include "SocketEntity.h"

namespace ClientServerChatApp {
    /**
     * Client object encapsulating the low level functionality of dealing with sockets
     */
    class Client: public LibSocket::ClientSocket<SocketSizeType> {
    private:
        static void run_client(Client* client);
    public:
        SmartConsole::Console* console;
        /// Main thread sends ip address, client thread receives it
        SyncPoint<std::string> sync_ip_address;
        /// Main thread sends port, client thread receives it
        SyncPoint<std::string> sync_port;
        /// Main thread sends username, client thread receives it
        SyncPoint<std::string> sync_username;
        /// Client thread sends registration results, main receives it
        SyncPoint<bool> sync_registered;
        /// Child thread sends Socket created successful, main receives it
        SyncPoint<bool> sync_socket_created;
        /// Child thread sends Socket connection successful, main receives it
        SyncPoint<bool> sync_socket_established;
        /// Ring buffer for outgoing messages
        Utilities::RingBuffer<std::string> send_buffer;
        explicit Client(SmartConsole::Console* _console);
        /**
         * Initialize client on separate thread
         * @return thread handle
         */
        std::thread initialize_client();
    };
}


#endif //CLIENTSERVERCHATAPP_CLIENT_H
