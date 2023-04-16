//
// Created by Robert Sale on 4/10/23.
//

#ifndef CLIENTSERVERCHATAPP_SYNCPOINT_H
#define CLIENTSERVERCHATAPP_SYNCPOINT_H

/**
 * Super totally awesome synchronization primitive
 * Meant for data to be consumed very quickly, or to trigger async events on other threads
 * Works like a reusable promise
 * @tparam T
 */
template<typename T>
class SyncPoint {
private:
    /// Data can be anything
    T* data;
    /// Protects access to data
    std::mutex mtx;
    /// Allows receiver to wait until data has been resolved
    std::condition_variable cv;
public:
    SyncPoint(): data(nullptr) {}
    ~SyncPoint() {
        delete data;
    }
    /**
     * Sends data to receiver
     * @param to the data
     */
    void resolve(T to) {
        UniqueLock lock{mtx};
        data = new T{to};
        cv.notify_all();
    }
    /**
     * Receives the data sent by resolve and deletes the data from the heap
     * @return
     */
    T retrieve() {
        UniqueLock lock{mtx};
        cv.wait(lock, [&]{ return data != nullptr; });
        T rv = *data;
        delete data;
        data = nullptr;
        return rv;
    }
};


#endif //CLIENTSERVERCHATAPP_SYNCPOINT_H
