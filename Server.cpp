//
// Created by Robert Sale on 4/9/23.
//

#include "Server.h"
#include "Commands.h"
#include "Logger.h"

namespace ClientServerChatApp {

    void Server::run_server(Server* server, std::string port) {
        std::function<void(std::string,Socket*)> receive_err_handler = [&](const std::string& payload, Socket* socket) {
            std::string msg = "[ERROR]: Failed to receive message";
            server->console->push_message(msg);
            Utilities::log(msg);
        };
        // Accept error handlers
        server->accept_handlers[LibSocket::SocketAcceptError::SUCCESS] = [&](std::shared_ptr<Socket> client) {
            if (server->_client_sockets.size() == server->_max_users) {
                client->full_send(MessageSignals::SRV_FULL(), {});
                return;
            }
            server->_client_sockets.push_back(client);
            std::string conn_msg = "[INFO]: A new client has connected!";
            server->console->push_message(conn_msg);
            client->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = [&](const std::string msg, Socket* client) {
                if (msg.empty()) return;
                if (msg.starts_with(Commands::REGISTER())) {
                    if (msg.size() == Commands::REGISTER().size()) {
                        client->full_send("[ERROR]: Missing username", {});
                        return;
                    }
                    std::string username = msg.substr(Commands::REGISTER().size());
                    server->users[client->get_fd()] = username;
                    std::string derp{"[INFO]: " + username + " has joined the chat!"};
                    server->broadcast(derp);
                    server->console->push_message(derp);
                    return;
                }
                if (msg.starts_with(Commands::EXIT())) {
                    auto message = std::string("[INFO]: " + server->users[client->get_fd()] + " disconnected.");
                    server->broadcast(message);
                    server->console->push_message(message);
                    for (size_t i = 0; i < server->_client_sockets.size(); i++) {
                        if (server->_client_sockets[i]->get_fd() == client->get_fd()) {
                            server->_client_sockets.erase(server->_client_sockets.begin() + i);
                            break;
                        }
                    }
                    return;
                }
                if (msg.starts_with(Commands::GET_LIST())) {
                    // TODO: Implement $getlist in the server
                    return;
                }
                if (msg.starts_with(Commands::GET_LOG())) {
                    // TODO: Implement $getlog in the server
                    return;
                }
                std::string response = server->users[client->get_fd()] + ": " + msg;
                server->broadcast(response);
                Utilities::log(response);
                server->console->push_message(response);
            };
            for (auto err : LibSocket::all_receive_errors) {
                client->receive_handlers[err] = receive_err_handler;
            }
            client->receive_handlers[LibSocket::SocketReceiveError::DISCONNECTING] = [&] (const std::string& payload, Socket* socket) {
//                std::string username{};
//                if (server->users.contains(socket->get_fd())) username = server->users[socket->get_fd()];
//                for(size_t i = 0; i < server->_client_sockets.size(); ++i) {
//                    if (socket->get_fd() == server->_client_sockets[i]->get_fd()) {
////                        delete server->_client_sockets[i];
////                        server->_client_sockets.erase(server->_client_sockets.begin() + i);
//                        break;
//                    }
//                }
//                if (username.empty()) return;
//                std::string msg = "[INFO]: " + username + " disconnected";
//                server->broadcast(msg);
            };
            Utilities::log(conn_msg);

        };
        std::function<void(std::shared_ptr<Socket>)> accept_err_handler = [&](std::shared_ptr<Socket> s) {
            std::string msg = "[ERROR]: Socket failed to be accepted.";
            server->console->push_message(msg);
            Utilities::log(msg);
        };
        for (auto err : LibSocket::all_accept_errors) server->accept_handlers[err] = accept_err_handler;
        // Create error handlers
        server->create_handlers[LibSocket::SocketCreateError::SUCCESS] = [&] {
            std::string msg = "[INFO]: Socket created.";
            server->console->push_message(msg);
            Utilities::log(msg);
            server->sync_socket_created.resolve(true);
        };
        std::function<void()> create_err_handler = [&] {
            std::string msg = "[ERROR]: Socket failed to be created.";
            server->console->shutdown.store(true);
            server->console->push_message(msg);
            Utilities::log(msg);
            server->sync_socket_created.resolve(false);
        };
        for (auto err : LibSocket::all_create_errors) { server->create_handlers[err] = create_err_handler; }
        // Listen error handlers
        server->listen_handlers[LibSocket::SocketListenError::SUCCESS] = [&] {
            std::string msg = "[INFO]: Server listening on port " + port;
            server->console->push_message(msg);
            Utilities::log(msg);
        };
        std::function<void()> listen_err_handler = [&] {
            std::string msg = "[ERROR]: Socket failed to listen on port " + port;
            server->console->shutdown.store(true);
            server->console->push_message(msg);
            Utilities::log(msg);
        };
        for (auto err : LibSocket::all_listen_errors) { server->listen_handlers[err] = listen_err_handler; }
        // Send error handlers
        server->send_handlers[LibSocket::SocketSendError::SUCCESS] = [&](const std::string& payload) {
//            std::string msg = "";
//            server->console->push_message(msg);
//            Utilities::log(msg);
        };
        server->send_handlers[LibSocket::SocketSendError::PIPE] = [&](const std::string& payload) {
            std::string msg = "[ERROR]: Message failed to send due to local socket being severed";
            server->console->push_message(msg);
            Utilities::log(msg);
        };
        // Receive error handlers
//        server->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = [&](const std::string& payload, Socket* socket) {
//
//        };
        for (auto err : LibSocket::all_receive_errors) server->receive_handlers[err] = receive_err_handler;


        // Start server in order
        server->create(LibSocket::SocketFamily::INET, LibSocket::Type::STREAM);
        server->bind_v4("127.0.0.1", port);
        server->listen();
        while(!server->console->shutdown.load()) {
            server->run_loop_timeout(2);
        }
    }

    void Server::broadcast(const std::string& message) {
        Utilities::log(message);
        for (auto client : _client_sockets) {
            client->full_send(message, {});
        }
    }

    void Server::run_loop(struct timeval* timeout) {
        FD_ZERO(&_read_fds);
        set_to_fds(_read_fds);
        int max_sd = _fd;
        for (int i = 0; i < _client_sockets.size(); ++i) {
            if (_client_sockets[i]->get_fd() > 0) _client_sockets[i]->set_to_fds(_read_fds);
            if (_client_sockets[i]->get_fd() > max_sd) max_sd = _client_sockets[i]->get_fd();
        }
        select(max_sd, timeout);
        if (fds_is_set(_read_fds)) accept(nullptr, nullptr);

        for (size_t i = 0; i < _client_sockets.size(); ++i) {
            if (_client_sockets[i]->fds_is_set(_read_fds)) { // client has sent something
                _client_sockets[i]->receive_str({});
            }
        }
    }
    void Server::run_loop_timeout() { run_loop(nullptr); }
    void Server::run_loop_timeout(size_t seconds, size_t microseconds) {
        struct timeval timeout;
        timeout.tv_usec = microseconds;
        timeout.tv_sec = seconds;
        run_loop(&timeout);
    }

    Server::Server(SmartConsole::Console *_console): LibSocket::ServerSocket<SocketSizeType>(), console(_console) {}

    std::thread Server::initialize_server(std::string port) {
        return std::thread{run_server, this, port};
    }
} // ClientServerChatApp