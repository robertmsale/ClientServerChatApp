//
// Created by Robert Sale on 4/9/23.
//

#include "Server.h"
#include "Commands.h"
#include "Logger.h"
#include "ShutdownTasks.h"
#include <fstream>
#include <getopt.h>

namespace ClientServerChatApp {

    void Server::run_server(Server* server, std::string port, std::string ip = "127.0.0.1") {
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
                if(!server->users.contains(client->get_fd())) {
                    client->full_send("[ERROR]: You must register to send messages!", {});
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
                    for (auto i = server->users.begin(); i != server->users.end(); i++) {
                        client->full_send(i->second, {});
                    }
                    client->full_send(MessageSignals::SRV_DONE_SEND(), {});
                    return;
                }
                if (msg.starts_with(Commands::GET_LOG())) {
                    std::string line;
                    UniqueLock lock{Utilities::logger_mtx};
                    std::ifstream file{Utilities::logger_file_path};
                    // send every line of the file
                    while(getline(file, line)) {
                        client->full_send(line, {});
                    }
                    client->full_send(MessageSignals::SRV_DONE_SEND(), {});
                    // let client deal with what to do next
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
                std::string username{};
                if (server->users.contains(socket->get_fd())) username = server->users[socket->get_fd()];
                for(size_t i = 0; i < server->_client_sockets.size(); ++i) {
                    if (socket->get_fd() == server->_client_sockets[i]->get_fd()) {
//                        delete server->_client_sockets[i];
//                        server->_client_sockets.erase(server->_client_sockets.begin() + i);
                        break;
                    }
                }
                if (username.empty()) return;
                std::string msg = "[INFO]: " + username + " disconnected";
                server->broadcast(msg);
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

        server->udp_socket.create(LibSocket::SocketFamily::INET, LibSocket::Type::DATAGRAM);
        int reuse = 1;
        setsockopt(server->udp_socket.get_fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
        setsockopt(server->udp_socket.get_fd(), SOL_SOCKET, SO_BROADCAST, &reuse, sizeof reuse);
        // Start server in order
        server->create(LibSocket::SocketFamily::INET, LibSocket::Type::STREAM);
        server->bind_v4(ip, port);
        server->listen();
        std::thread udp_broadcaster{[&] {
            struct sockaddr_in broadcast;
            std::memset(&broadcast, 0, sizeof broadcast);
            broadcast.sin_family = AF_INET;
            broadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);
            auto nport = htons(std::stoi(port));
            server->udp_socket.bind((sockaddr*)&broadcast, sizeof broadcast);
            broadcast.sin_port = nport;
            struct sockaddr_in connection_addr;
            std::memset(&connection_addr, 0, sizeof connection_addr);
            connection_addr.sin_family = AF_INET;
            if (inet_pton(AF_INET, ip.c_str(), &connection_addr.sin_addr) <= 0) {
                server->console->push_message("[ERROR]: Something went wrong with the IP address");
                ShutdownTasks::instance().execute();
                return;
            }
            connection_addr.sin_port = nport;
            while (!server->console->shutdown.load()) {
                auto res = sendto(server->udp_socket.get_fd(), &connection_addr, sizeof connection_addr, 0, (const sockaddr *)(&broadcast), sizeof broadcast);
                auto err = errno;
                if (res == -1) {
                    server->console->push_message("[ERROR]: Failed to send connection details");
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }};
        while(!server->console->shutdown.load()) {
            server->run_loop_timeout(2);
        }
        udp_broadcaster.join();
    }

    void Server::broadcast(const std::string& message) {
        for (const auto& client : _client_sockets) {
            client->full_send(message, {});
        }
    }

    void Server::run_loop(struct timeval* timeout) {
        FD_ZERO(&_read_fds);
        set_to_fds(_read_fds);
        int max_sd = _fd;
        for (const auto & _client_socket : _client_sockets) {
            if (_client_socket->get_fd() > 0) _client_socket->set_to_fds(_read_fds);
            if (_client_socket->get_fd() > max_sd) max_sd = _client_socket->get_fd();
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

    Server::Server(SmartConsole::Console *_console): LibSocket::ServerSocket<SocketSizeType>(), console(_console), udp_socket() {}

    std::thread Server::initialize_server(const std::string& port, const std::string& ip) {
        return std::thread{run_server, this, port, ip};
    }
} // ClientServerChatApp