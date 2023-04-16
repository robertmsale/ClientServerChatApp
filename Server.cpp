//
// Created by Robert Sale on 4/9/23.
//

#include "Server.h"
#include "Commands.h"

namespace ClientServerChatApp {
    void Server::run_server(Server* server, std::string port) {
        if (server->create_socket() != SocketSignal::SUCCESS) {
            server->console->push_message("[ERROR]: Failed to create socket");
            server->sync_socket_created.resolve(false);
            return;
        }
        server->sync_socket_created.resolve(true);
        server->start_listening(port);
    }

    SocketSignal Server::start_listening(std::string port) {
        int opt = 1;
        int ready;
        auto signal = SocketSignal::SUCCESS;
        Utilities::DeferExec defer{[&] {active_signal.store(signal);}};
//        if (setsockopt(io, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof opt)) {
//            console->push_message("[ERROR]: Failed to set socket options.");
//            signal = SocketSignal::BIND_ERROR;
//            return signal;
//        }
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(std::stoi(port));

        // Bind, listen, accept, and use select to multiplex TCP clients.
        if (bind(io, (struct sockaddr *)&address, sizeof(address)) < 0) {
            console->push_message("[ERROR]: Failed to bind.");
            signal = SocketSignal::BIND_ERROR;
            return signal;
        }
        if (listen(io, 3) < 0) {
            console->push_message("[ERROR]: Failed to listen.");
            signal = SocketSignal::LISTEN_ERROR;
            return signal;
        }
        fd_set readfds;
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        while (!console->shutdown.load()) {
            FD_ZERO(&readfds);
            FD_SET(io, &readfds);
            int max_sd = io;
            for (int i = 0; i < client_sockets.size(); ++i) {
                if (client_sockets[i] > 0) FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_sd) max_sd = client_sockets[i];
            }
            auto select_rv = select(max_sd+1, &readfds, nullptr, nullptr, nullptr);
            if (select_rv < 0 && errno != EINTR) {
                console->push_message("[ERROR]: Failed to select.");
                signal = SocketSignal::SELECT_ERROR;
                return signal;
            }
            if (select_rv == 0) {
                continue;
            }
            if (FD_ISSET(io, &readfds)) {
                auto client_socket = accept(io, nullptr, nullptr);
                if (client_socket < 0) {
                    console->push_message("[ERROR]: Failed to accept client socket.");
                    signal = SocketSignal::ACCEPT_ERROR;
                    continue;
                }
//                if (client_sockets.size() < MAX_USERS()) {
//
//                }
                FD_SET(client_socket, &readfds);
                std::string message = "[INFO]: New client connected";
                console->push_message(message);
                client_sockets.push_back(client_socket);
            }

            for (size_t i = 0; i < client_sockets.size(); ++i) {
                if (FD_ISSET(client_sockets[i], &readfds)) {
                    char buffer[SocketMaxMessageSize()];
                    memset(buffer, 0, SocketMaxMessageSize());
                    SocketSizeType size;
                    auto bytes_read = read(client_sockets[i], &size, sizeof(size)); // first read size
                    if (bytes_read == 0) { // user disconnecting
                        std::string message{"[INFO]: " + users[client_sockets[i]] + " is disconnecting."};
                        broadcast(message);
                        close(client_sockets[i]);
                        FD_CLR(client_sockets[i], &readfds);
                        client_sockets.erase(client_sockets.begin() + i--);
                        continue;
                    }
                    else {
                        read(client_sockets[i], buffer, size);
                        auto received = std::string(buffer);
                        if (received.starts_with(Commands::REGISTER())) {
                            std::string username{received.substr(Commands::REGISTER().size(), received.size())};
                            users[client_sockets[i]] = username;
                            broadcast("[INFO]: " + username + " joined the chat!");
                            continue;
                        }
                        if (received.starts_with(Commands::EXIT())) {
                            auto message = std::string("[INFO]: " + users[client_sockets[i]] + " disconnected.");
                            auto size = (SocketSizeType)received.size();
                            send(client_sockets[i], &size, sizeof size, 0);
                            send(client_sockets[i], received.c_str(), received.size(), 0);
                            client_sockets.erase(client_sockets.begin() + i--);
                            console->push_message(message);
                            broadcast(message);
                            continue;
                        }
                        if (received.starts_with(Commands::GET_LIST())) {

                        }
                        if (received.starts_with(Commands::GET_LOG())) {

                        }
                        auto message = std::string(users[client_sockets[i]] + ": " + received);
                        console->push_message(message);
                        broadcast(message);
                    }
                }
            }
        }

        return SocketSignal::SUCCESS;
    }

    void Server::broadcast(std::string message) {
        for (auto client : client_sockets) {
            auto size = (SocketSizeType)message.size();
            send(client, &size, sizeof size, 0);
            send(client, message.c_str(), message.size(), 0);
        }
    }

    Server::Server(SmartConsole::Console *_console) {
        console=_console;
    }

    std::thread Server::initialize_server(std::string port) {
        return std::thread{run_server, this, port};
    }
} // ClientServerChatApp