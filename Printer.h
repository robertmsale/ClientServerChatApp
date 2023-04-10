//
// Created by Robert Sale on 4/8/23.
//

#ifndef CLIENTSERVERCHATAPP_PRINTER_H
#define CLIENTSERVERCHATAPP_PRINTER_H

namespace Printer {
    /// Mutex for managing print (recursive to handle variadic template print)
    static std::recursive_mutex MTX;

    /// Default print case does nothing (in recursion, this is the final call to print)
    static void print() {}

    /// Variadic print function: Synchronization point between threads using recursive mutex.
    template<typename Arg, typename ...Args>
    static void print(Arg arg, Args... args) {
        MTX.lock();
        std::cout << arg;
        print(args...);
        MTX.unlock();
    }

    /// Default sprint case returns empty string
    static std::string sprint() { return ""; }

    /// Variadic sprint: Feeds all arguments into std::stringstream and returns a string for ultra convenient string making
    template<typename Arg, typename ...Args>
    static std::string sprint(Arg arg, Args... args) {
        std::stringstream ss;
        ss << arg << sprint(args...);
        return ss.str();
    }
}


#endif //CLIENTSERVERCHATAPP_PRINTER_H
