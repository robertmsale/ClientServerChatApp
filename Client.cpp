//
// Created by Robert Sale on 4/8/23.
//

#include "Client.h"
#include "Commands.h"
#include "ShutdownTasks.h"


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
            ShutdownTasks::instance().execute();
            client->console->refresh_text.resolve(true);
        };
        client->connect_handlers[LibSocket::SocketConnectError::SUCCESS] = [&] {
            client->sync_socket_established.resolve(true);
        };
        for(auto err: LibSocket::all_connect_errors) client->connect_handlers[err] = [&] {
            client->sync_socket_created.resolve(false);
            client->console->push_message("[ERROR]: Failed to connect to server.");
            ShutdownTasks::instance().execute();
        };
        // create socket
        // Try to connect to server
        std::function<void(const std::string&, Socket*)> recv_success_handler = [&](const std::string& payload, Socket* socket) {
            if (payload.empty()) return;
            if (payload == MessageSignals::SRV_FULL()) {
                client->console->push_message("[WARNING]: Server is full. Exiting.");
                ShutdownTasks::instance().execute();
                return;
            }
            client->console->push_message(payload);
        };
        client->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = recv_success_handler;
        client->receive_handlers[LibSocket::SocketReceiveError::DISCONNECTING] = [&](const std::string& _payload_, Socket* socket) {
//            client->console->push_message("[INFO]: Server is shutting down. Exiting.");
//            ShutdownTasks::instance().execute();
        };
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
        std::mutex glmtx;
        std::vector<std::string> getlist_res;
        client->sync_registered.resolve(true);
        client->console->input_hooks.push([&](char* buff) {
            if (buff[0] == 'q') {
                client->console->input_hooks.set_active(0, true);
                client->console->input_hooks.set_active(1, false);
                client->console->render_hooks.set_active(1, true);
                client->console->render_hooks.set_active(0, true);
                client->console->refresh_text.resolve(true);
                client->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = recv_success_handler;
            }
            }, 1);
        client->console->input_hooks.set_active(1, false);
//        client->console->render_hooks.push([&] (SmartConsole::Console* c) {
//            for (auto i: getlist_res) {
//                std::cout << i << '\n';
//            }
//            std::string msg{"Press q to go back"};
//            SmartConsole::SetCursorPosition(std::cout, c->console_width()-msg.size()-1, c->console_height());
//            std::cout << msg << std::flush;
//        }, 2);
//        client->console->render_hooks.set_active(2, false);
        std::thread sender{[&] {
            while (!client->console->shutdown.load()) {
                std::string payload = client->send_buffer.rx();
                if (payload.starts_with(Commands::GET_LOG()) || payload.starts_with(Commands::GET_LIST())) {
//                    UniqueLock lock{glmtx};
                    // disable normal hooks
                    client->console->input_hooks.set_active(0, false);
                    client->console->render_hooks.set_active(1, false);
                    client->console->render_hooks.set_active(0, false);
                    // enable q capture hook
                    client->console->input_hooks.set_active(1, true);

                    SmartConsole::Clear(std::cout);
                    // every new message received will get shoved right into the alternate buffer

                    client->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = [&](const std::string& payload, Socket*){
                        if (payload == MessageSignals::SRV_DONE_SEND()) {
                            client->receive_handlers[LibSocket::SocketReceiveError::SUCCESS] = recv_success_handler;
                            std::cout << "Press q to return";
                            return;
                        }
                        if (!payload.empty()) std::cout << payload << '\n';
                    };
                    client->full_send(payload, {});
                    continue;
                }
                client->full_send(payload, {});
            }
        }};
        while(!client->console->shutdown.load()) {
            if (!client->console->alternate_buffer.load()) client->receive_str({});
        }
        sender.join();
    }

}
