//
// Created by Robert Sale on 4/8/23.
//

// TODO: Delete me

#ifndef CLIENTSERVERCHATAPP_SOCKETHELPERS_H
#define CLIENTSERVERCHATAPP_SOCKETHELPERS_H

namespace SocketHelpers {
    struct CreateClientReturnValue {
        int socket;
        int status;

    };
    /// Takes server information, creates socket, and connects client to server.
    /// Returns: int representing socket file descriptor
    int create_client_socket(std::string ip, int port);
}

#endif //CLIENTSERVERCHATAPP_SOCKETHELPERS_H
