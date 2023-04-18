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
    class Client: public SocketEntity {
    private:
        static void run_client(Client* client);
    public:
        /// Main thread sends ip address, client thread receives it
        SyncPoint<std::string> sync_ip_address;
        /// Main thread sends port, client thread receives it
        SyncPoint<std::string> sync_port;
        /// Main thread sends username, client thread receives it
        SyncPoint<std::string> sync_username;
        /// Client thread sends registration results, main receives it
        SyncPoint<bool> sync_registered;
        /// Ring buffer for outgoing messages
        Utilities::RingBuffer<std::string> send_buffer;
        explicit Client(SmartConsole::Console* _console);
        ~Client();
        /**
         * Connects the Socket to the server. Make sure the Socket has been created before connecting.
         * @param _to IP address of server
         * @param _port Port number of server
         * @return Signal result of attempted connection
         */
        SocketSignal connect_to_server(std::string _to, std::string _port);
        /**
         * Sends a message to the server
         * @param _message string message
         * @return Signal result of attempted send
         */
        SocketSignal send_msg(std::string _message);
        /**
         * Receives a message from the server
         * @return Signal and string message received from server
         */
        ReceiveMessage recv_msg();
        /**
         * Initialize client on separate thread
         * @return thread handle
         */
        std::thread initialize_client();
    };
}


#endif //CLIENTSERVERCHATAPP_CLIENT_H
