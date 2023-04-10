//
// Created by Robert Sale on 4/9/23.
//

#include "SocketEntity.h"
#include "Printer.h"

namespace ClientServerChatApp {
    SocketEntity::SocketEntity(): io(-1) {}

    bool SocketEntity::socket_created() const { return io >= 0; }

    SocketSignal SocketEntity::create_socket() {
        io = socket(AF_INET, SOCK_STREAM, 0);
        if (io < 0) {
            Printer::print("[ERROR]: Failed to create socket.\n");
            return SocketSignal::STARTUP_ERROR;
        }
        return SocketSignal::SUCCESS;
    }

    SocketSignal SocketEntity::resolve_response(ssize_t _response_code) {
        switch (_response_code) {
            case 0:
                Printer::print("[Signal]: Shutting down.\n");
                return SocketSignal::SHUTDOWN;
            case -1:
                Printer::print("[Signal]: Disconnecting.\n");
                return SocketSignal::DISCONNECT;
            default: return SocketSignal::SUCCESS;
        }
    }
} // ClientServerChatApp