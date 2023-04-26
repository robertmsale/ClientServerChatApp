//
// Created by Robert Sale on 4/9/23.
//

#ifndef CLIENTSERVERCHATAPP_SERVER_H
#define CLIENTSERVERCHATAPP_SERVER_H

#include "Console.h"

namespace ClientServerChatApp {
    /**
     * Server object encapsulating the low level functionality of working with sockets
     */
    class Server: public LibSocket::ServerSocket<SocketSizeType> {
    private:
        using Socket = LibSocket::Socket<SocketSizeType>;
        /**
         * Function created when running the server thread
         * @param server Pointer to server object
         * @param port Port number to listen on
         */
        static void run_server(Server* server, std::string port, std::string ip);

        std::vector<std::shared_ptr<Socket>> _client_sockets;

        LibSocket::ServerSocket<SocketSizeType> udp_socket;
    public:
        /// Child thread sends Socket created successful, main receives it
        SyncPoint<bool> sync_socket_created;
        /// Child thread sends Socket connection successful, main receives it
        SyncPoint<bool> sync_socket_established;
        SmartConsole::Console* console;
        /**
         * Sends message to all connected clients
         * @param message
         */
        void broadcast(const std::string& message);
        /**
         * Constructs the Server object
         * @param _console to handle rendering to the screen
         */
        explicit Server(SmartConsole::Console* _console);
        /// The function that gets run inside a loop that handles constant polling of the sockets
        void run_loop(struct timeval* timeout);
        void run_loop_timeout(size_t seconds, size_t microseconds = 0);
        void run_loop_timeout();
        std::map<int, std::string> users;
        std::thread initialize_server(const std::string& port, const std::string& ip);
    };
} // ClientServerChatApp

#endif //CLIENTSERVERCHATAPP_SERVER_H
