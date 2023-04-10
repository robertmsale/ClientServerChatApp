//
// Created by Robert Sale on 8/29/21.
//

#ifndef BLACKJACKGAME_CONSOLE_H
#define BLACKJACKGAME_CONSOLE_H
#include "SocketEntity.h"
#include "SyncPoint.h"
#if defined(_WIN32)
#include <Windows.h>
#else
#include <termios.h>
#endif

#include <vector>

/**
 * Console functionality I wrote a long time ago for a C++ version of the PG2 Blackjack game
 */
namespace SmartConsole {
    /**
     * Font decorators
     */
    enum class Decorations {
        Default = 0,
        Bold = 1,
        NoBold = 22,
        Underline = 4,
        NoUnderline = 24
    };
    /**
     * Foreground colors
     */
    enum class FGColor {
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
        Extended = 38,
        Default = 39,
        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97
    };
    /**
     * Background Colors
     */
    enum class BGColor {
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
        Extended = 48,
        Default = 49,
        BrightBlack = 100,
        BrightRed = 101,
        BrightGreen = 102,
        BrightYellow = 103,
        BrightBlue = 104,
        BrightMagenta = 105,
        BrightCyan = 106,
        BrightWhite = 107
    };
    namespace CharacterSet {
        const char LineDraw = '0';
        const char ASCII = 'B';
    }
    namespace Line {
        const char CBR = 'j';
        const char CTR = 'k';
        const char CTL = 'l';
        const char CBL = 'm';
        const char UNI = 'n';
        const char HRL = 'q';
        const char LCC = 't';
        const char RCC = 'u';
        const char BCC = 'v';
        const char TCC = 'w';
        const char VTL = 'x';
    }
    void SetFGColor(std::ostream& ss, FGColor color);
    void SetBGColor(std::ostream& ss, BGColor color);
    void SetColorFMT(std::ostream& ss, Decorations c);
    void SetColors(std::ostream& ss, FGColor fg, BGColor bg);
    void SetColors(std::ostream& ss, BGColor bg, FGColor fg);
    void SetCursorPosition(std::ostream& ss, int x, int y);
    void SetCharacterSet(std::ostream& ss, char s);
    void ResetFormat(std::ostream& ss);
    void Clear(std::ostream& ss);
    void GetTerminalSize(int& width, int& height);
    void PrintPretty(std::ostream& ss, char *message, int x = 0, int y = 0, int msgSize = 0);
    void ReadKey(char *output);
    char *ReadLine();
    void InitializeTerminal();
    void SaveCursorPosition(std::ostream& ss);
    void RestoreCursorPosition(std::ostream& ss);

    /**
     * Class responsible for managing standard input and output
     */
    class Console {
#pragma region LayoutProperties
    private:
        /// Atomic console width
        std::atomic<int> _console_width{-1};
        /// Atomic console height
        std::atomic<int> _console_height{-1};
    private:
        /// How far the user has scrolled using the arrow keys
        std::atomic<int> _message_window_scroll_position{0};

        /// When scrolled to the bottom, autoscroll gets used
        std::atomic<bool> _message_window_autoscroll{true};

        /// Triggers layout refresh
        std::atomic<bool> _refresh_layout{false};
#pragma endregion
#pragma region StoredProperties
    public:

        /// Tells threads when to gracefully shut down
        std::atomic<bool> shutdown{false};

        /// Easy-peasy 1-1 sync to refresh the text
        SyncPoint<bool> refresh_text;

        /// Mutex for the messages vector
        /// TODO: Decide if this is still necessary
        std::mutex messages_mtx;

        /// Condition variable for the messages mutex
        /// TODO: Decide if this is still necessary
        std::condition_variable messages_cv;

        /// Vector of messages that get printed to the screen
        std::vector<std::string> messages;

        /// Mutex for input buffer
        /// TODO: Most likely not necessary and should be removed
        std::mutex stdin_buff_mtx;

        /// Mutex for input buffer
        /// TODO: Most likely not necessary and should be removed
        std::condition_variable stdin_buff_cv;

        /// Standard Input Buffer used by input capture thread
        char buffer[SocketMaxMessageSize()];

        /// Position of the cursor for user input at any given time
        size_t buff_position{0};

        /// Buffer promise
        /// When this promise gets resolved it sends user input to the main thread to be processed
        std::promise<std::string>* buffer_promise{new std::promise<std::string>};

        /// String stream for generating the console window
        /// using this rather than std::cout so the whole screen buffer can be generated before sending to stdout
        std::stringstream ss;
#pragma endregion
#pragma region ComputedProperties
        /// Get the console width from the private atomic field
        int console_width() const;
        /// Get the console height from the private atomic field
        int console_height() const;
        /**
         * Message window size
         * @return height of the message window taking into account surrounding elements
         */
        int msg_window_size() const;
#pragma endregion
#pragma region AppTitle
        /**
         * App Title!
         * constexpr so that the compiler can generate this at compile time
         * @return vector of each row of the app title
         */
        static constexpr std::vector<std::string> title() {
            std::vector<std::string> rv;

            rv.emplace_back(R"(______________        _____     _______                 )");
            rv.emplace_back(R"(__  ____/__  /_______ __  /_    ___    |_______________ )");
            rv.emplace_back(R"(_  /    __  __ \  __ `/  __/    __  /| |__  __ \__  __ \)");
            rv.emplace_back(R"(/ /___  _  / / / /_/ // /_      _  ___ |_  /_/ /_  /_/ /)");
            rv.emplace_back(R"(\____/  /_/ /_/\__,_/ \__/      /_/  |_|  .___/_  .___/ )");
            rv.emplace_back(R"(                                       /_/     /_/      )");

            return rv;
        }

        /**
         * Title Width
         * constexpr, once the actual title is generated, this gets the width of the title at compile time
         * @return width of title
         */
        static constexpr size_t title_width() {
            return title()[0].size();
        }

        /**
         * Title Height
         * constexpr, once the actual title is generated, this gets the height of the title at compile time
         * @return width of title
         */
        static constexpr size_t title_height() {
            return title().size();
        }

        /**
         * Title Mask
         * This is the mask which is used to color each character of the title
         * @return [x,y] mask of terminal escape sequences
         */
        static constexpr std::vector<std::vector<std::string>> title_mask() {
            std::vector<std::vector<std::string>> rv;
            for (auto s : title()) {
                std::vector<std::string> inner;
                for (auto c : s) {
                    std::string ns;
                    switch (c) { // Foreground
                        case '_': ns = "\x1b[31m"; // Red
                        default: ns = "\x1b[32m";  // Green
                    }
                    inner.emplace_back(ns);
                }
                rv.emplace_back(inner);
            }
            return rv;
        }
#pragma endregion
#pragma region LayoutMethods
        /// Gets console size at this very moment (typically when messages are received or user inputs data)
        void update_console_size();
#pragma endregion
#pragma region DrawMethods
        /// Draw the title to screen
        void draw_title();
        /// Draws only the window which the messages will be printed in
        void draw_message_window();
        /// Draw the actual messages
        void draw_messages();
        /// Draw the scroll bar on the right side of the message window
        void draw_scroll_bar();
        /// Draw the input buffer at the bottom of the screen
        /// TODO: Probably remove this one
        void draw_input_buffer();
        /// Clear Message Window
        /// TODO: Probably remove this one
        void clear_message_window();
#pragma endregion
#pragma region ThreadMethods
    private:
        /// Renderer thread function
        static void run_renderer(Console* console);
        /// Input capture thread function
        static void run_input_capture(Console* console);

    public:
        /// Initialize the renderer thread
        std::thread initialize_renderer();
        /// Initialize the input capture thread
        std::thread initialize_input_capture();
#pragma endregion
    };
}

#endif //BLACKJACKGAME_CONSOLE_H
