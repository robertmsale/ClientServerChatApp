//
// Created by Robert Sale on 4/9/23.
//

#ifndef CLIENTSERVERCHATAPP_COMMANDS_H
#define CLIENTSERVERCHATAPP_COMMANDS_H


namespace ClientServerChatApp {
    namespace Commands {
        constexpr std::string REGISTER() { return "$register "; }
        constexpr std::string EXIT() { return "$exit"; }
        constexpr std::string GET_LIST() { return "$getlist"; }
        constexpr std::string GET_LOG() { return "$getlog"; }
    }
}

#endif //CLIENTSERVERCHATAPP_COMMANDS_H
