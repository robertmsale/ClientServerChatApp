//
// Created by Robert Sale on 8/29/21.
//
#include "Console.h"

#pragma region OldSmartConsoleCode
void SetGraphicsRendition(std::ostream& ss, const int *rg, int size, char terminator = 'm') {
    ss << (char)0x1b << '[';
    int i = 0;
    while (size > 0) {
        ss << *(rg + i++);
        if (size-- != 1) ss << ';';
    }
    ss << terminator;
}

void SmartConsole::SetFGColor(std::ostream& ss, SmartConsole::FGColor color) {
    int c = (int)color;
    int *ptr = &c;
    SetGraphicsRendition(ss, ptr, 1);
}

void SmartConsole::SetBGColor(std::ostream& ss, SmartConsole::BGColor color) {
    int c = (int)color;
    int *ptr = &c;
    SetGraphicsRendition(ss, ptr, 1);
}

void SmartConsole::SetColorFMT(std::ostream& ss, Decorations color) {
    int c = (int)color;
    int *ptr = &c;
    SetGraphicsRendition(ss, ptr, 1);
}

void SmartConsole::SetColors(std::ostream& ss, BGColor bg, FGColor fg) {
    int *cs = new int[2];
    *cs = (int)fg;
    *(cs + 1) = (int)bg;
    SetGraphicsRendition(ss, cs, 2);
    delete[](cs);
}

void SmartConsole::SetColors(std::ostream& ss, FGColor fg, BGColor bg) {
    SetColors(ss, bg, fg);
}

void SmartConsole::SetCursorPosition(std::ostream& ss, int x, int y) {
    int *cs = new int[2];
    *cs = y;
    *(cs + 1) = x;
    SetGraphicsRendition(ss, cs, 2, 'H');
    delete[] cs;
}

void SmartConsole::SetCharacterSet(std::ostream& ss, char s) {
    ss << (char)0x1b << '(' << s;
}

void SmartConsole::Clear(std::ostream& ss) {
    ss << "\x1b[2J\x1b[H";
}
void SmartConsole::ResetFormat(std::ostream& ss) {
    ss << (char) 0x1b << "[0m"; // UnChange color
}

void SmartConsole::GetTerminalSize(int &width, int &height) {
//#ifdef TIOCGSIZE
//    struct ttysize ts;
//    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
//    width = ts.ts_cols;
//    height = ts.ts_lines;
//#elif defined(TIOCGWINSZ)
//    struct winsize ts;
//    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
//    width = ts.ws_col;
//    height = ts.ws_row;
//#endif
}

void SmartConsole::PrintPretty(std::ostream& ss, char *message, int x, int y, int msgSize) {
    if (x < 1 || y < 1) {
        int dx, dy;
        GetTerminalSize(dx, dy);
        if (x < 1) x = dx / 2 - msgSize / 2;
        if (y < 1) y = dx / 2;
    }
    SetCursorPosition(ss, x, y);
    for (int i = 0; i < msgSize; ++i) {
        ss << *(message + i);
    }
}

void SmartConsole::ReadKey(char *output) {
//#if defined(_WIN32)
//    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
//    DWORD NumInputs = 0;
//    DWORD InputsRead = 0;
//    bool running = true;
//
//    INPUT_RECORD irInput;
//
//    GetNumberOfConsoleInputEvents(hInput, &NumInputs);
//
//    ReadConsoleInput(hInput, &irInput, 1, &InputsRead);
//#else
//    struct termios t;
//    tcgetattr(0, &t);
//    t.c_lflag &= ~ICANON;
//    tcsetattr(0, TCSANOW, &t);
//    *output = (char)std::cin.get();
//#endif
    std::cin.ignore();
    std::cin.get(*output);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

char *SmartConsole::ReadLine() {
    char* output = new char[SocketMaxMessageSize()];
    std::cin.getline(output, SocketMaxMessageSize());
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return output;
}
void SmartConsole::SaveCursorPosition(std::ostream& ss) {
    ss << "\x1b[s";
}
void SmartConsole::RestoreCursorPosition(std::ostream& ss) {
    ss << "\x1b[u";
}

#if defined(_WIN32) || defined(WIN32)
//#include <stdio.h>
#include <wchar.h>
#include <windows.h>

int SmartConsole::InitializeTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwOriginalOutMode = 0;
    DWORD dwOriginalInMode = 0;
    if (!GetConsoleMode(hOut, &dwOriginalOutMode))
    {
        return false;
    }
    if (!GetConsoleMode(hIn, &dwOriginalInMode))
    {
        return false;
    }

    DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    DWORD dwRequestedInModes = ENABLE_VIRTUAL_TERMINAL_INPUT;

    DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
    if (!SetConsoleMode(hOut, dwOutMode))
    {
        // we failed to set both modes, try to step down mode gracefully.
        dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
        if (!SetConsoleMode(hOut, dwOutMode))
        {
            // Failed to set any VT mode, can't do anything here.
            return -1;
        }
    }

    DWORD dwInMode = dwOriginalInMode | ENABLE_VIRTUAL_TERMINAL_INPUT;
    if (!SetConsoleMode(hIn, dwInMode))
    {
        // Failed to set VT input mode, can't do anything here.
        return -1;
    }

    return 0;
}
#else


void SmartConsole::InitializeTerminal() {
    char* locale = setlocale(LC_ALL, "");
    std::locale lollocale(locale);
    setlocale(LC_ALL, locale);
    std::wcout.imbue(lollocale);
}

#endif
#pragma endregion

namespace SmartConsole {
    Console::Console(std::string commands) {
        memset(buffer, 0, SocketMaxMessageSize());
        _commands = commands;
    }
#pragma region Console::
#pragma region ComputedProperties
    int Console::msg_window_size() const {
        return (int)console_height() - (int)title_height() - 3;
    }
    int Console::console_width() const {
        return _console_width.load();
    }

    int Console::console_height() const {
        return _console_height.load();
    }

#pragma endregion
#pragma region LayoutMethods
    void Console::update_console_size() {
        struct ttysize ts;
        ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
        auto width = ts.ts_cols;
        auto height = ts.ts_lines;
        if (width != console_width() || height != console_height()) {
            _console_width.store(std::max((int)width, 60));
            _console_height.store(std::max((int)height, 20));
            _refresh_layout.store(true);
        }
    }
#pragma endregion
#pragma region DrawMethods
    void Console::draw_title() {
        size_t center_x = console_width() / 2;
        size_t left = center_x - title_width() / 2;
        for (size_t y = 0; y < title_height(); y++) {
            for (size_t x = 0; x < title_width(); x++) {
                SetCursorPosition(ss, left + x, y);
                ss << title_mask()[y][x] << title()[y][x];
            }
        }
        SmartConsole::ResetFormat(ss);
    }

    void Console::draw_message_window() {
        SmartConsole::SetCursorPosition(ss, 0, (int)title_height());
        SmartConsole::SetCharacterSet(ss, SmartConsole::CharacterSet::LineDraw);
        ss << SmartConsole::Line::CTL << SmartConsole::Line::HRL;
        SmartConsole::SetCharacterSet(ss, SmartConsole::CharacterSet::ASCII);
        ss << " Messages ";
        SmartConsole::SetCharacterSet(ss, SmartConsole::CharacterSet::LineDraw);
        for (int i = 0; i < console_width() - 13; i++) {
            ss << SmartConsole::Line::HRL;
        }
        ss << SmartConsole::Line::CTR;
        for (int i = 0; i < std::max((int)(console_height() - title_height() - 3), 0); ++i) {
            ss << SmartConsole::Line::VTL << "\x1b[" << console_width() << 'G' << SmartConsole::Line::VTL;
        }
        ss << SmartConsole::Line::CBL;
        for (int i = 0; i < console_width()-2; ++i) {
            ss << SmartConsole::Line::HRL;
        }
        ss << SmartConsole::Line::CBR;
        SmartConsole::SetCharacterSet(ss, SmartConsole::CharacterSet::ASCII);
        SmartConsole::SetFGColor(ss, SmartConsole::FGColor::BrightBlue);
        ss << "~~ Commands: " << _commands;
        SmartConsole::ResetFormat(ss);
    }

    void Console::draw_messages() {
        int scroll_pos = _message_window_scroll_position.load();
        SmartConsole::SetCursorPosition(ss, 3, (int)title_height() + 1);
        int message_window_height = msg_window_size();
        for (int i = 0; i < message_window_height; ++i) {
            if (i + scroll_pos > messages.size()) break;
            if (messages[i + scroll_pos].starts_with("[INFO]:")) {
                SmartConsole::SetFGColor(ss, SmartConsole::FGColor::BrightBlue);
            } else if (messages[i + scroll_pos].starts_with("[WARNING]:")) {
                SmartConsole::SetFGColor(ss, SmartConsole::FGColor::BrightYellow);
            } else if (messages[i + scroll_pos].starts_with("[ERROR]:")) {
                SmartConsole::SetFGColor(ss, SmartConsole::FGColor::BrightRed);
            }
            ss << messages[i + scroll_pos] << "\x1b[1B\x1b[3G";
            SmartConsole::ResetFormat(ss);
        }
    }

        void Console::draw_scroll_bar() {
        int msg_win_height = msg_window_size()-2;
        int scroll_pos = _message_window_scroll_position.load();
        int max_scroll_pos = std::max((int)messages.size() - msg_win_height, 0);
        int bar_height = (int)((float) msg_win_height - (float)msg_win_height * ((float)max_scroll_pos / (float)messages.size()));
        int top_of_msg_win = (int)title_height()+2;
        int top_of_bar = (int)std::lerp((float)(top_of_msg_win), (float)(top_of_msg_win + msg_win_height - bar_height), (float)scroll_pos / ((float)max_scroll_pos));
        SmartConsole::SetBGColor(ss, SmartConsole::BGColor::BrightBlack);
        for(int i = 0; i < bar_height; ++i) {
            SmartConsole::SetCursorPosition(ss, console_width()-1, top_of_bar + i);
            ss << ' ';
        }
        SmartConsole::SetCharacterSet(ss, SmartConsole::CharacterSet::LineDraw);
        SmartConsole::SetCursorPosition(ss, console_width()-1, top_of_msg_win-1);
        ss << (char)96;
        SmartConsole::SetCursorPosition(ss, console_width()-1, console_height()-3);
        ss << (char)96;
        SmartConsole::SetCharacterSet(ss, SmartConsole::CharacterSet::ASCII);
        SmartConsole::ResetFormat(ss);
    }


    void Console::draw_input_buffer() {
        SmartConsole::SetCursorPosition(ss, 0, console_height()-1);
        std::cout << "$ " << buffer;
    }

    void Console::clear_message_window() {
        for (auto i = title_height()+1; i < console_height() - title_height() - 3; ++i) {
            SmartConsole::SetCursorPosition(ss, 2, i);
            ss << "\x1b[" << console_width() - 2 << "X";
        }
    }
#pragma endregion
#pragma region ThreadMethods
    std::thread Console::initialize_renderer() {
//        char* locale = setlocale(LC_ALL, "");
//        std::locale lollocale(locale);
//        setlocale(LC_ALL, locale);
//        std::wcout.imbue(lollocale);
        std::thread rv{run_renderer, this};
        return rv;
    }

    void Console::run_renderer(Console* console) {
        while(!console->shutdown.load()) {
            console->update_console_size();
//            if (console->console_height() <= 0) continue;
//            if (console->_refresh_layout.load()) {
                SmartConsole::Clear(console->ss);
                console->draw_title();
                console->draw_message_window();
//                console->_refresh_layout.store(false);
//            }
            console->messages_mtx.lock();
            if (console->_message_window_autoscroll.load()) {
                console->_message_window_scroll_position.store(std::max((int)console->messages.size() - (console->console_height() - (int)title_height() - 4), 0));
            }
//            console->clear_message_window();
            console->draw_scroll_bar();
            console->draw_messages();
            console->messages_mtx.unlock();
            SmartConsole::SetCursorPosition(console->ss, 1, console->console_height());
            SmartConsole::SetFGColor(console->ss, SmartConsole::FGColor::Green);
            console->ss << ">\x1b[" << strlen(console->buffer) << "C";
            SmartConsole::ResetFormat(console->ss);
            std::cout << console->ss.str() << std::flush;
            console->ss = std::stringstream{};
            console->refresh_text.retrieve();
        }
    }

    void Console::run_input_capture(SmartConsole::Console *console) {
        while(!console->shutdown.load()) {
            char buff[16];
            memset(buff, 0, 16);
            read(STDIN_FILENO, &buff, 16);
            if (buff[0] == (char)127) { // backspace
                if (console->buff_position == 0) continue;
                console->buffer[--console->buff_position] = '\0';
            } else if(buff[0] == '\n') { // new line
                if (strlen(console->buffer) == 0) continue;
                if (strcmp(console->buffer, "$exit") == 0) {

                    console->shutdown.store(true);
                    console->refresh_text.resolve(true);
                }
                console->ring_buffer.tx(std::string{console->buffer});
                console->buff_position = 0;
                memset(console->buffer, 0, SocketMaxMessageSize());
            } else if (buff[0] >= ' ' && buff[0] <= '~') {
                strcpy(console->buffer + (console->buff_position++), buff);
            } else if (strcmp(buff, "\x1b[A") == 0) { // up arrow
                console->_message_window_autoscroll.store(false);
                if (console->_message_window_scroll_position.load() == 0) continue;
                atomic_fetch_sub(&console->_message_window_scroll_position,1);
                console->refresh_text.resolve(true);
            } else if (strcmp(buff, "\x1b[B") == 0) { // down arrow
                if (console->_message_window_autoscroll.load()) continue;
                auto end = console->messages.size() - (console->msg_window_size());
                auto pos = atomic_fetch_add(&console->_message_window_scroll_position,1);
                if (pos == end) console->_message_window_autoscroll.store(true);
                console->refresh_text.resolve(true);
            }
            SmartConsole::ResetFormat(std::cout);
            std::cout << "\x1b[" << console->console_height() << ";3H\x1b[" << console->console_width() << "P" << console->buffer << std::flush;
        }
    }

    std::thread Console::initialize_input_capture() {
        return std::thread{run_input_capture, this};
    }

    void Console::push_message(const std::string &message) {
        UniqueLock lock{messages_mtx};
        messages.push_back(message);
        refresh_text.resolve(true);
    }

#pragma endregion

#pragma endregion
}
