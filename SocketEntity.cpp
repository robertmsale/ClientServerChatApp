//
// Created by Robert Sale on 4/9/23.
//

#include "SocketEntity.h"

namespace ClientServerChatApp {
    SocketEntity::SocketEntity(): io(-1) {}

    bool SocketEntity::socket_created() const { return io >= 0; }

    SocketSignal SocketEntity::create_socket() {
        io = socket(AF_INET, SOCK_STREAM, 0);
        auto rv = SocketSignal::SUCCESS;
        Utilities::DeferExec defer{[&]() {active_signal.store(rv);}};
        if (io < 0) {
            UniqueLock lock{console->messages_mtx};
            console->messages.emplace_back("[ERROR]: Failed to create socket.");
            console->refresh_text.resolve(true);
            rv = SocketSignal::STARTUP_ERROR;
        }
        return rv;
    }

    SocketSignal SocketEntity::resolve_response(SmartConsole::Console* console, ssize_t _response_code) {
        auto rv = SocketSignal::SUCCESS;
        switch (_response_code) {
            case 0: {
                UniqueLock lock{console->messages_mtx};
                console->messages.emplace_back("[INFO]: Shutting down.");
                console->refresh_text.resolve(true);
                rv = SocketSignal::SHUTDOWN;
                break;
            }
            case -1: {
                UniqueLock lock{console->messages_mtx};
                console->messages.emplace_back("[INFO]:  Disconnecting.");
                console->refresh_text.resolve(true);
                rv = SocketSignal::DISCONNECT;
                break;
            }
            default: break;
        }
        return rv;
    }

    SocketEntity::~SocketEntity() {
        close(io);
    }
} // ClientServerChatApp