//
// Created by Robert Sale on 4/8/23.
//

#include "Client.h"


namespace ClientServerChatApp {

    Client::Client(SmartConsole::Console* _console) {
        console = _console;
    }

    std::thread Client::initialize_client() {
        return std::thread{Client::run_client, this};
    }

    void Client::run_client(Client* client) {
        client->create_handlers[LibSocket::SocketCreateError::SUCCESS] = [&] {
            client->sync_socket_created.resolve(true);
        };
        for(auto err: LibSocket::all_create_errors) client->create_handlers[err] = [&] {
            client->sync_socket_created.resolve(false);
            client->console->push_message("[ERROR]: Failed to create socket. Shutting down now.");
//            client->console->shutdown.store(true);
            client->console->refresh_text.resolve(true);
        };
        client->connect_handlers[LibSocket::SocketConnectError::SUCCESS] = [&] {
            client->sync_socket_established.resolve(true);
        };
        for(auto err: LibSocket::all_connect_errors) client->connect_handlers[err] = [&] {
            client->sync_socket_created.resolve(false);
            client->console->push_message("[ERROR]: Failed to connect to server.");
        };
        // create socket
        // Try to connect to server
        client->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = [&](const std::string& payload, Socket* socket) {
            if (payload.empty()) return;
            if (payload == MessageSignals::SRV_FULL()) {
                client->console->push_message("[WARNING]: Server is full. Exiting.");
//                client->console->shutdown.store(true);
                client->console->refresh_text.resolve(true);
                return;
            }
            client->console->push_message(payload);
        };
//        client->receive_handlers[LibSocket::SocketReceiveError::DISCONNECTING] = [&](const std::string& _payload_, Socket* socket) {
//            client->console->push_message("[INFO]: Server is shutting down. Exiting.");
////            client->console->shutdown.store(true);
//            client->console->refresh_text.resolve(true);
//        };
//        for (auto err: LibSocket::all_receive_errors) client->receive_handlers[err]
        for (auto err: LibSocket::all_send_errors) client->send_handlers[err] = [&] (const std::string& payload) {
            client->console->push_message("[WARNING]: Message failed to send");
        };
        client->create(LibSocket::SocketFamily::INET, LibSocket::Type::STREAM);
        if (client->console->shutdown.load()) return;
        auto ip = client->sync_ip_address.retrieve();
        auto port = client->sync_port.retrieve();
        client->connect_v4(ip, port);
        if (client->console->shutdown.load()) return;
        // Initiate message loop
        auto username = client->sync_username.retrieve();
        client->full_send("$register " + username, {});
        auto res = client->receive_str({});
        if (client->console->shutdown.load()) {
            client->sync_registered.resolve(false);
            return;
        }
        client->sync_registered.resolve(true);
        std::thread sender{[&] {
            while (!client->console->shutdown.load()) {
                client->full_send(client->send_buffer.rx(), {});
            }
        }};
        while(!client->console->shutdown.load()) {
            client->receive_str({});
        }
        sender.join();
    }

}
