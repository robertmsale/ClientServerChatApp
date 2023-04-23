//
// Created by Robert Sale on 4/16/23.
//

#include "Logger.h"
#include <sstream>
#include <fstream>
#include <ctime>
// Basically as long as whoever starts the server doesn't enter this secret hash a
// temporary logger file will be generated with consideration to today's date and time.
#define LOGGER_FILE_SECRET_HASH "uWJ0CU7dfbFUecigBPzjnPaC8vDKAuwZ89ybB0xRoR3fXJrIewPePnQZ6M2HCPIY7cH88334g+v0RfK0eJZc=="


namespace Utilities {
    std::string logger_file_path{LOGGER_FILE_SECRET_HASH};
    std::mutex logger_mtx{};
    void log(std::string message) {
        UniqueLock lock{logger_mtx};
        // default file is /tmp/chatapp<current date/time>.log
        if (logger_file_path == LOGGER_FILE_SECRET_HASH) {
            std::time_t t = std::time(nullptr);
            std::tm* now = std::localtime(&t);
            std::stringstream ss;
            ss  << "/tmp/chatapp"
                << (now->tm_year + 1900)
                << '-'
                << (now->tm_mon + 1)
                << '-'
                << now->tm_mday
                << '.'
                << now->tm_hour
                << '-'
                << now->tm_min
                << '-'
                << now->tm_sec
                << ".log";
            logger_file_path = ss.str();
        }
        std::fstream fs{logger_file_path, std::ios_base::app | std::ios_base::out};
        fs << message << '\n';
        fs.close();
//        in_memory_log.push_back(message);
    }
} // Utilities

// Paranoid undef just in case
#undef LOGGER_FILE_SECRET_HASH