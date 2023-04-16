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
        std::vector<int> client_sockets;
        void broadcast(std::string message);
        explicit Server(SmartConsole::Console* _console);
        std::map<int, std::string> users;
        std::thread initialize_server(std::string port);
    };
} // ClientServerChatApp

#endif //CLIENTSERVERCHATAPP_SERVER_H
