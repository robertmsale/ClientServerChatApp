//
// Created by Robert Sale on 4/9/23.
//

#ifndef CLIENTSERVERCHATAPP_SERVER_H
#define CLIENTSERVERCHATAPP_SERVER_H

#include "SocketEntity.h"

namespace ClientServerChatApp {
    /**
     * Server object encapsulating the low level functionality of working with sockets
     */
    class Server: public SocketEntity {
    private:
        /**
         * Function created when running the server thread
         * @param server Pointer to server object
         * @param port Port number to listen on
         */
        static void run_server(Server* server, std::string port);
        /**
         * Have server start listening
         * @param port the port to listen on
         * @return signal result from listening
         */
        SocketSignal start_listening(std::string port);
        static constexpr int MAX_USERS() {return 10;};
    public:
        /// Vector of file descriptors representing accepted client sockets
        std::vector<int> client_sockets;
        /**
         * Sends message to all connected clients
         * @param message
         */
        void broadcast(std::string message);
        /**
         * Constructs the Server object
         * @param _console to handle rendering to the screen
         */
        explicit Server(SmartConsole::Console* _console);
        std::map<int, std::string> users;
        std::thread initialize_server(std::string port);
    };
} // ClientServerChatApp

#endif //CLIENTSERVERCHATAPP_SERVER_H
