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
    class Client: SocketEntity {
    private:
        std::string username;
        bool registered;
    public:
        Client();
        ~Client();
        /**
         * Connects the socket to the server. Make sure the socket has been created before connecting.
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
    };
}


#endif //CLIENTSERVERCHATAPP_CLIENT_H
