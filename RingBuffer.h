//
// Created by Robert Sale on 4/11/23.
//

#ifndef CLIENTSERVERCHATAPP_RINGBUFFER_H
#define CLIENTSERVERCHATAPP_RINGBUFFER_H

#include <mutex>
#include <queue>
#include <condition_variable>

namespace Utilities {
    /**
     * Totally awesome and super cool ring buffer algorithm
     * allows for one or more threads to enqueue data to another thread
     * @tparam T
     */
    template<typename T>
    class RingBuffer {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<T> buffer;
    public:
        /**
         * Transmit data to consumer
         * @param data to send
         */
        void tx(T data) {
            std::unique_lock<std::mutex> lock{mtx};
            buffer.push(data);
            cv.notify_all();
        }
        /**
         * Receive data from producers
         * @return data
         */
        T rx() {
            std::unique_lock<std::mutex> lock{mtx};
            if (buffer.empty()) {
                cv.wait(lock, [&] { return !buffer.empty();});
            }
            auto rv = buffer.front();
            buffer.pop();
            return rv;
        }
    };
}

#endif //CLIENTSERVERCHATAPP_RINGBUFFER_H
