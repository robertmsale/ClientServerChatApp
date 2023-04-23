//
// Created by Robert Sale on 4/19/23.
//

#ifndef CLIENTSERVERCHATAPP_SHUTDOWNTASKS_H
#define CLIENTSERVERCHATAPP_SHUTDOWNTASKS_H

namespace ClientServerChatApp {

    class ShutdownTasks {
    private:
        ShutdownTasks();
        std::vector<std::function<void()>> tasks;
    public:
        std::mutex mtx;
        static ShutdownTasks& instance();
        void push_task(const std::function<void()>& task);
        void execute();
    };

} // ClientServerChatApp

#endif //CLIENTSERVERCHATAPP_SHUTDOWNTASKS_H
