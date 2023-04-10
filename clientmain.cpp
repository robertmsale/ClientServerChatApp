#include "Console.h"
#include "Commands.h"

#define PHASE 1

void disable_echo(struct termios* orig);

void prompt_for_connection_details(std::vector<std::string>* messages);

int main() {
    struct termios orig_tios;
    tcgetattr(STDIN_FILENO, &orig_tios);
    disable_echo(&orig_tios);

    SmartConsole::Console console;
    SmartConsole::Clear(std::cout);
#if PHASE == 1
    console.messages.emplace_back("Welcome to Chat App!");
    console.messages.emplace_back("To get started, please enter the server IP Address.");
#elif PHASE == 2
    console.messages.emplace_back("Welcome to Chat App!");
    console.messages.emplace_back("To get started, use the $register command to register your username with the server.");
#endif
//    for(int i = 0; i < 30; ++i) {
//        console.messages.emplace_back("To get started, use the $register command to register your username with the server.");
//    }
    console.update_console_size();
    std::thread renderer_thread = console.initialize_renderer();
    std::thread input_capture_thread = console.initialize_input_capture();
    std::string username;
    bool registered = false;
    std::string ip_address;
    std::string port;
#if PHASE == 1
    std::regex ip_regex{"(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])"};
    std::regex port_regex{"([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])"};
#endif
    while(!console.shutdown.load()) {
        auto future = console.buffer_promise->get_future();
        auto user_msg = future.get();
        auto old_promise = console.buffer_promise;
        console.buffer_promise = new std::promise<std::string>;
        delete old_promise;
#if PHASE == 1
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
        if (!registered) {
            if (!user_msg.starts_with(ClientServerChatApp::Commands::REGISTER())) {
                UniqueLock lock{console.messages_mtx};
                console.messages.emplace_back("[ERROR]: You have to register before entering commands or messages to the chat room");
                console.refresh_text.resolve(true);
            }
            continue;
        }
    }



    renderer_thread.join();
    input_capture_thread.join();

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_tios);
    return 0;
}

void disable_echo(struct termios* orig) {
    struct termios tios;
    memcpy(&tios, orig, sizeof tios);
//    tios.c_iflag &=
//        ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
//    tios.c_oflag &= ~OPOST;
//    tios.c_lflag = ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tios.c_lflag = 0;
//    tios.c_cflag &= ~(CSIZE | PARENB);
//    tios.c_cflag |= CS8;
//    tios.c_cc[VMIN] = 0;
//    tios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tios);
}