//
// Created by Robert Sale on 4/8/23.
//

#include "Client.h"


namespace ClientServerChatApp {
    SocketSignal Client::connect_to_server(std::string _to, std::string _port) {
        auto rv = SocketSignal::SUCCESS;
        Utilities::DeferExec defer{[&] {active_signal.store(rv);}};
        // Prepare connection info
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(stoi(_port));
        // Try converting ip address
        if (inet_pton(AF_INET, _to.c_str(), &serv_addr.sin_addr) <= 0) {
            UniqueLock lock{console->messages_mtx};
            console->messages.emplace_back("[ERROR]: Invalid IP address.");
            console->refresh_text.resolve(true);
            rv = SocketSignal::ADDRESS_ERROR;
            return rv;
        }
        // Try connecting
        if ( connect( io, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0) {
            UniqueLock lock{console->messages_mtx};
            console->messages.emplace_back("[ERROR]: Failed to connect.");
            console->refresh_text.resolve(true);
            rv = SocketSignal::CONNECT_ERROR;
        }
        return rv;
    }

    Client::Client(SmartConsole::Console* _console) {
        console = _console;
    }

    Client::~Client() {
        close(io);
    }

    std::thread Client::initialize_client() {
        return std::thread{Client::run_client, this};
    }

    SocketSignal Client::send_msg(std::string _message) {
        // Establish and confirm message size
        auto size = (SocketSizeType)_message.size();
        // define return value
        auto rv = SocketSignal::SUCCESS;
        // ensure that return value is stored as active signal when returning
        Utilities::DeferExec defer{[&] {active_signal.store(rv);}};

        // You tried!
        if (size == 0 || _message.size() > SocketMaxMessageSize()) {
            UniqueLock lock{console->messages_mtx};
            console->messages.emplace_back("[ERROR]: Message too long to send.");
            console->refresh_text.resolve(true);
            rv = SocketSignal::PARAMETER_ERROR;
            return rv;
        }

        // Send size of message to client
        auto send_res = send(io, &size, sizeof(SocketSizeType), 0);
        rv = resolve_response(console, send_res);
        if (rv != SocketSignal::SUCCESS) return rv;

        // Send actual message
        send_res = send(io, _message.c_str(), _message.size(), 0);
        rv = resolve_response(console, send_res);

        return rv;
    }

    ReceiveMessage Client::recv_msg() {
        // Variables
        char buffer[SocketMaxMessageSize()];
        memset(buffer, 0, SocketMaxMessageSize());
        SocketSizeType size;

        // Receive size
        auto recv_res = recv(io, (SocketSizeType*)&size, sizeof(SocketSizeType), 0);
        auto recv_code = resolve_response(console, recv_res);
        Utilities::DeferExec defer{[&] {active_signal.store(recv_code);}};
        if (recv_code != SocketSignal::SUCCESS) return {"", recv_code};

        // Receive full message
        recv_res = recv(io, buffer, size, 0);
        recv_code = resolve_response(console, recv_res);
        if (recv_code != SocketSignal::SUCCESS) return {"", recv_code};

        return {
            buffer,
            recv_code
        };
    }

    void Client::run_client(Client* client) {
        // Try to create socket
        if (client->create_socket() != SocketSignal::SUCCESS) {
            client->sync_socket_created.resolve(false);
            return;
        }
        client->sync_socket_created.resolve(true);
        // Try to connect to server
        std::string ip = client->sync_ip_address.retrieve(); // main thread validates this
        std::string port = client->sync_port.retrieve(); // main thread validates this
        if (client->connect_to_server(ip, port) != SocketSignal::SUCCESS) {
            client->sync_socket_established.resolve(false);
            return;
        }
        client->sync_socket_established.resolve(true);
        // Initiate message loop
        while(!client->console->shutdown.load()) {
            // Start by registering to server
            if (client->send_msg("$register " + client->sync_username.retrieve()) != SocketSignal::SUCCESS) {
                client->sync_registered.resolve(false);
                continue;
            }
            // Receive response from server
            auto msg = client->recv_msg();
            if (msg.code != SocketSignal::SUCCESS) {
                client->sync_registered.resolve(false);
            }
            // Handle if server is full
            if (msg.message == MessageSignals::SRV_FULL()) {
                client->console->push_message(msg.message);
                client->sync_registered.resolve(false);
            }
            client->sync_registered.resolve(true);
            std::atomic<bool> receiver_running{true};
            // Initiate message receiver thread
            std::thread receiver{[&] {
                while(!client->console->shutdown.load() /*&& client->active_signal.load() == SocketSignal::SUCCESS*/) {
                    auto msg = client->recv_msg();
                    client->active_signal.store(msg.code);
                    switch (msg.code) {
                        case SocketSignal::SHUTDOWN:
                        case SocketSignal::DISCONNECT: {
                            client->console->push_message("[INFO]: Closing connection to server.");
                            client->console->refresh_text.resolve(true);
                            receiver_running.store(false);
                            return;
                        }
                        case SocketSignal::SUCCESS: {
                            client->console->push_message(msg.message);
                            client->console->refresh_text.resolve(true);
                        }
                        default: break;
                    }
                }
            }};
            while(!client->console->shutdown.load() && receiver_running.load()) {
                auto msg = client->send_buffer.rx();
                client->send_msg(msg);
            }
            // At this point the connection to the server has been severed or shutdown has been initiated
            // if shutdown, start from the beginning of the loop, otherwise exit
            // Join receiver before shutting down
            client->console->shutdown.store(true); // tell other services to shut down now
            client->console->refresh_text.resolve(true);
            client->send_buffer.tx("$exit");
            receiver.join();
        }

    }

}
