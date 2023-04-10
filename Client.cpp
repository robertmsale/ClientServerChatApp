//
// Created by Robert Sale on 4/8/23.
//

#include "Client.h"
#include "Printer.h"


namespace ClientServerChatApp {
    Client::Client(): username("Guest"), registered(false) {}

    Client::~Client() {
        close(io);
    }

    SocketSignal Client::connect_to_server(std::string _to, std::string _port) {
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(stoi(_port));
        if (inet_pton(AF_INET, _to.c_str(), &serv_addr.sin_addr) <= 0) {
            Printer::print("[ERROR]: Invalid IP address.\n");
            return SocketSignal::ADDRESS_ERROR;
        }
        if ( connect( io, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0) {
            Printer::print("[ERROR]: Failed to connect.\n");
            return SocketSignal::CONNECT_ERROR;
        }
        return SocketSignal::SUCCESS;
    }

    SocketSignal Client::send_msg(std::string _message) {
        // Establish and confirm message size
        auto size = _message.size();
        if (size == 0 || size > SocketMaxMessageSize()) {
            Printer::print("[ERROR]: Message too long to send.\n");
            return SocketSignal::PARAMETER_ERROR;
        }

        // Send size of message to client
        auto send_res = send(io, (SocketSizeType*)&size, sizeof(SocketSizeType), 0);
        auto send_code = resolve_response(send_res);
        if (send_code != SocketSignal::SUCCESS) return send_code;

        // Send actual message
        send_res = send(io, _message.c_str(), _message.size(), 0);
        send_code = resolve_response(send_res);

        return send_code;
    }

    ReceiveMessage Client::recv_msg() {
        // Variables
        char buffer[SocketMaxMessageSize()];
        SocketSizeType size;

        // Receive size
        auto recv_res = recv(io, (SocketSizeType*)&size, sizeof(SocketSizeType), 0);
        auto recv_code = resolve_response(recv_res);
        if (recv_code != SocketSignal::SUCCESS) return {"", recv_code};

        // Receive full message
        recv_res = recv(io, buffer, size, 0);
        recv_code = resolve_response(recv_res);
        if (recv_code != SocketSignal::SUCCESS) return {"", recv_code};

        return {
            buffer,
            recv_code
        };
    }
}
