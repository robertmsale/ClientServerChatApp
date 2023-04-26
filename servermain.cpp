//
// Created by Robert Sale on 4/9/23.
//

#include "Console.h"
#include "Commands.h"
#include "Server.h"
#include "Logger.h"

void disable_echo(struct termios* orig);

int main(int argc, char** argv) {
    std::string ip{"127.0.0.1"};
    if (argc < 2) {
        std::cout << "Usage: server <port number> [ip address] [log file]\n"
                     "\tExamples:\n"
                     "\t\tserver 33420\n"
                     "\t\tserver 33420 127.0.0.1\n"
                     "\t\tserver 12345 messages.log\n" << std::endl;
        return 0;
    }
    if (!std::regex_match(argv[1], LibSocket::port_regex)) {
        std::cout << "Usage: server <port number> [ip address] [log file]\n"
                     "              ^^^^^^^^^^^^^\n"
                     "The port you entered was invalid. Please try again.";
        return 1;
    }
    if (argc > 2) {
        if (!std::regex_match(std::string{argv[2]}, LibSocket::ipv4_regex)) {
            std::cout << "Usage: server <port number> [ip address] [log file]\n"
                         "                            ^^^^^^^^^^^^\n"
                         "The IP you entered was invalid. Please try again.";
            return 1;
        }
        ip = argv[2];
    }
    if (argc == 4) {
        Utilities::logger_file_path = std::string{argv[3]};
    }
    struct termios orig_tios;
    tcgetattr(STDIN_FILENO, &orig_tios);
    disable_echo(&orig_tios);
    std::string port{argv[1]};
    SmartConsole::Clear(std::cout);
    SmartConsole::Console console{"$exit"};
    ClientServerChatApp::Server server(&console);
    console.messages.emplace_back("Welcome to Chat App server!");
    std::thread renderer = console.initialize_renderer();
    std::thread input_capturer = console.initialize_input_capture();
    std::thread server_thread = server.initialize_server(port, ip);
    while (!console.shutdown.load()) std::this_thread::sleep_for(std::chrono::seconds(2));
    server_thread.join();
    input_capturer.join();
    renderer.join();
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