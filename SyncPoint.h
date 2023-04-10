//
// Created by Robert Sale on 4/10/23.
//

#ifndef CLIENTSERVERCHATAPP_SYNCPOINT_H
#define CLIENTSERVERCHATAPP_SYNCPOINT_H


template<typename T>
class SyncPoint {
private:
    T* data;
    std::mutex mtx;
    std::condition_variable cv;
public:
    SyncPoint(): data(nullptr) {}
    ~SyncPoint() {
        delete data;
    }
    void resolve(T to) {
        UniqueLock lock{mtx};
        data = new T{to};
        cv.notify_all();
    }
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
