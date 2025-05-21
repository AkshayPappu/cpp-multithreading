#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class BoundedBuffer {
    private:
        size_t capacity;
        std::queue<T> buffer;
        std::mutex mu;
        std::condition_variable not_full;
        std::condition_variable not_empty;
        bool stopped;
        
    public:
        explicit BoundedBuffer(size_t capacity) : capacity(capacity), stopped(false) {}

        void push(const T& item) {
            // lock mutex 
            std::unique_lock<std::mutex> lock(mu);

            // wait for buffer to not be full
            not_full.wait(lock, [this] { return buffer.size() < capacity || stopped; });
            if (stopped) { return; }

            // add item to buffer
            buffer.push(item);

            // notify consumer
            not_empty.notify_one();
            return;
        }
        bool pop(T& item) {
            // lock mutex
            std::unique_lock<std::mutex> lock(mu);

            // wait for buffer to not be empty
            not_empty.wait(lock, [this] { return !buffer.empty() || stopped; });
            if (buffer.empty()) { return false; }

            // get item from buffer
            item = buffer.front();
            buffer.pop();

            // notify producer
            not_full.notify_one();

            // return item
            return true;
        }

        void stop() {
            // lock mutex
            std::unique_lock<std::mutex> lock(mu);

            // set stop flag
            stopped = true;

            // notify all threads
            not_full.notify_all();
            not_empty.notify_all();
            return;
        }
};