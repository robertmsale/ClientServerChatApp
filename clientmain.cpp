#include "Console.h"
#include "Commands.h"
#include "Client.h"
#include "ShutdownTasks.h"

#define PHASE 2

void disable_echo(struct termios* orig);

//void prompt_for_connection_details(std::vector<std::string>* messages);

int main() {
    // Get original terminal input/output structure
    struct termios orig_tios;
    tcgetattr(STDIN_FILENO, &orig_tios);
    // Disable echo. This allows me to control all keyboard input by the user. (also disables ctrl+c unfortunately)
    disable_echo(&orig_tios);
    ClientServerChatApp::ShutdownTasks::instance().push_task([&] {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_tios);
    });

    SmartConsole::Console console{"$register, $exit, $getlist, $getlog"};
    SmartConsole::Clear(std::cout);
    // Create initial messages
#if PHASE == 1
    console.messages.emplace_back("Welcome to Chat App!");
    console.messages.emplace_back("To get started, please enter the server IP Address.");
#elif PHASE == 2
    console.messages.emplace_back("Welcome to Chat App!");
//    console.messages.emplace_back("To get started, use the $register command to register your username with the server.");
#endif
    // initialize console size
    console.update_console_size();
    // create thread vars
    std::thread renderer_thread = console.initialize_renderer();
    std::thread input_capture_thread = console.initialize_input_capture();
    std::thread client_thread;
    Utilities::DeferExec defer_console_cleanup{[&] {
        renderer_thread.join();
        input_capture_thread.join();
    }};
    // create client vars
    ClientServerChatApp::Client client{&console};
    std::string username;
    bool registered = false;
    std::string ip_address;
    std::string port;
#if PHASE == 1
    // user input validation
    std::regex ip_regex{"(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])"};
    std::regex port_regex{"([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])"};
#else
    {
        console.push_message("Waiting for connection details...");
        LibSocket::Socket<SocketSizeType> client_socket;
        client_socket.create(LibSocket::SocketFamily::INET, LibSocket::Type::DATAGRAM);

        sockaddr_in client_addr;
        std::memset(&client_addr, 0, sizeof client_addr);
        socklen_t client_len{sizeof client_addr};
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        client_addr.sin_port = htons(12345);
        ::bind(client_socket.get_fd(), (sockaddr*)&client_addr, sizeof client_addr);
        sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof server_addr);
        timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        setsockopt(client_socket.get_fd(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
        socklen_t clisize = sizeof client_addr;
        auto res = ::recvfrom(client_socket.get_fd(), &server_addr, sizeof server_addr, 0, (sockaddr*)&client_addr, &clisize);
        if (res < 0) {
            console.push_message("[ERROR]: Failed to retrieve connection info. Shutting down.");
            ClientServerChatApp::ShutdownTasks::instance().execute();
            renderer_thread.join();
            input_capture_thread.join();
            return 1;
        }
        client_thread = client.initialize_client();
        client.sync_addr.resolve(server_addr);
    }
    Utilities::DeferExec defer_client_cleanup{[&] {client_thread.join();}};
#endif
    ClientServerChatApp::ShutdownTasks::instance().push_task([&] {
        console.shutdown.store(true);
        console.ring_buffer.tx("$exit");
    });
    while(!console.shutdown.load()) {
        // receive user input
        auto user_msg = console.ring_buffer.rx();
#if PHASE == 1
        // Collect IP address info
        if (ip_address.empty()) {
            if(std::regex_match(user_msg, ip_regex)) {
                ip_address = user_msg;
                UniqueLock lock{console.messages_mtx};
                console.messages.emplace_back("[INFO]: " + ip_address);
                console.messages.emplace_back("Excellent! Now enter the port number.");
                console.refresh_text.resolve(true);
            } else {
                UniqueLock lock{console.messages_mtx};
                console.messages.emplace_back("[ERROR]: You entered an invalid IP address. Please try again.");
                console.refresh_text.resolve(true);
            }
            continue;
        }
        // Collect port information
        if (port.empty()) {
            if(std::regex_match(user_msg, port_regex)) {
                port = user_msg;
                UniqueLock lock{console.messages_mtx};
                console.messages.emplace_back("[INFO]: " + port);
                console.messages.emplace_back("Alright, now all you have to do is use the $register command to join the server.");
                console.refresh_text.resolve(true);
            } else {
                UniqueLock lock{console.messages_mtx};
                console.messages.emplace_back("[ERROR]: You entered an invalid Port number. Please try again.");
                console.refresh_text.resolve(true);
            }
            continue;
        }
#endif
        // Begin registration
        if (!registered) {
            if (!user_msg.starts_with(ClientServerChatApp::Commands::REGISTER())) {
                UniqueLock lock{console.messages_mtx};
                console.messages.emplace_back("[ERROR]: You have to register before entering commands or messages to the chat room");
                console.refresh_text.resolve(true);
                continue;
            }
            username = user_msg.substr(10);
            client.sync_username.resolve(username);
            auto res = client.sync_socket_created.retrieve();
            if (!res) continue;
            res = client.sync_socket_established.retrieve();
            if (!res) continue;
            registered = client.sync_registered.retrieve();
            continue;
        }

        // at this point client is registered so begin echoing user input to server
        client.send_buffer.tx(user_msg);
    }

    return 0;
}


void disable_echo(struct termios* orig) {
    struct termios tios;
    memcpy(&tios, orig, sizeof tios);
    // commented lines were for testing options
//    tios.c_iflag &=
//        ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
//    tios.c_oflag &= ~OPOST;
//    tios.c_lflag = ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tios.c_lflag = 0; // disables echoing and canonical user input
//    tios.c_cflag &= ~(CSIZE | PARENB);
//    tios.c_cflag |= CS8;
//    tios.c_cc[VMIN] = 0;
//    tios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);
}