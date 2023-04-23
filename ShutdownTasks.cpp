//
// Created by Robert Sale on 4/19/23.
//

#include "ShutdownTasks.h"

namespace ClientServerChatApp {
    ShutdownTasks::ShutdownTasks(): tasks{} {}
    ShutdownTasks& ShutdownTasks::instance() {
        static ShutdownTasks st;
        return st;
    }
    void ShutdownTasks::push_task(const std::function<void()>& task) {
        UniqueLock lock{mtx};
        tasks.push_back(task);
    }
    void ShutdownTasks::execute() {
        UniqueLock lock{mtx};
        for(const auto& t: tasks) t();
    }
} // ClientServerChatApp