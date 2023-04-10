//
// Created by Robert Sale on 4/9/23.
//

#include "SocketHelpers.h"
#include "Printer.h"

int SocketHelpers::create_client_socket(std::string ip, int port) {
    int sockfd;
    struct sockaddr_in serv_addr{};
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        Printer::print("[ERROR]: Client failed to create socket.");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        Printer::print("[ERROR]: IP Address invalid.");
        return -1;
    }


}