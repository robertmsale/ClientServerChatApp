//
// Created by Robert Sale on 4/9/23.
//

#include "Console.h"
#include "Commands.h"
#include "Server.h"

void disable_echo(struct termios* orig);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: server <port number> [log file]\n"
                     "\tExamples:\n"
                     "\t\tserver 33420\n"
                     "\t\tserver 12345 messages.log\n" << std::endl;
        return 0;
    }
    std::string port{argv[1]};
//    std::string logFile{argc == 3 ? "messages.log" : argv[2]};
    SmartConsole::Clear(std::cout);
    SmartConsole::Console console{"$exit"};
    ClientServerChatApp::Server server(&console);
    console.messages.emplace_back("Welcome to Chat App server!");
    std::thread renderer = console.initialize_renderer();
    std::thread input_capturer = console.initialize_input_capture();
    std::thread server_thread = server.initialize_server(port);
    server_thread.join();
    console.shutdown.store(true);
    console.refresh_text.resolve(true);
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