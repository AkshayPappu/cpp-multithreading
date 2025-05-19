#pragma once
#include <atomic>
#include <vector>

template <typename T>
struct Slot {
    std::atomic<size_t> sequence;
    T value;
};

template <typename T>
class LockFreeQueue {
    private:
        std::vector<Slot<T>> queue;
        std::atomic<size_t> head;
        std::atomic<size_t> tail;
        size_t capacity;
    public:
        LockFreeQueue(size_t capacity) : queue(capacity), capacity(capacity), head(0), tail(0) {
            // initialize sequence numbers for each slot
            for (size_t i = 0; i < capacity; ++i) {
                queue[i].sequence.store(i, std::memory_order_relaxed);
            }
        };
        
        // enqueue an item into the queue
        bool enqueue(const T& item) {
            // atomically load the current tail and increment the pointer
            size_t index = tail.fetch_add(1, std::memory_order_relaxed);

            // load the slot at the current tail index and check if it's available
            Slot<T>& slot = queue[index % capacity];
            while (slot.sequence.load(std::memory_order_acquire) != index) {
                std::this_thread::yield();
            }
            
            // enqueue the item and update the sequence number
            slot.value = item;
            slot.sequence.store(index + 1, std::memory_order_release);
            return true;
        }

        // dequeue an item from the queue
        bool dequeue(T& item){
            // atomically load the current head and increment the pointer
            size_t index = head.fetch_add(1, std::memory_order_relaxed);

            // load the slot at the current head index and check if it's available
            Slot<T>& slot = queue[index % capacity];
            while (slot.sequence.load(std::memory_order_acquire) != index + 1) {
                std::this_thread::yield();
            }

            // dequeue the item and update the sequence number
            item = slot.value;
            slot.sequence.store(index + capacity, std::memory_order_release);
            return true;
        }
};