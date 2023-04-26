//
// Created by Robert Sale on 4/12/23.
//

#ifndef CLIENTSERVERCHATAPP_DEFEREXEC_H
#define CLIENTSERVERCHATAPP_DEFEREXEC_H

#include <functional>
#include <utility>
namespace Utilities {
    /**
     * This object takes a function and runs it on destruction.
     * This is a secure and reliable way of deferring execution of something until the end of a function call.
     * Use this if you want to guarantee something happens no matter what branch in a function gets executed.
     */
    class DeferExec {
        /// Function to execute upon destruction
        std::function<void()> func;
    public:
        explicit DeferExec(std::function<void()> _func): func(std::move(_func)), cancel{false} {}
        bool cancel;
        ~DeferExec() {
            if (!cancel) func();
        }
    };
}

#endif //CLIENTSERVERCHATAPP_DEFEREXEC_H
