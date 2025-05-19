#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <mutex>
#include "LockFreeQueue.h"

constexpr int NUM_PRODUCERS = 4;
constexpr int NUM_CONSUMERS = 4;
constexpr int ITEMS_PER_PRODUCER = 1000;
constexpr int QUEUE_CAPACITY = 1024;
constexpr int TOTAL_ITEMS = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

std::atomic<bool> done(false);
std::atomic<int> total_consumed{0};
std::mutex print_mutex;


void producer(LockFreeQueue<int>& queue, int id) {
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int value = id * 1000000 + i;
        while (!queue.enqueue(value)) {
            std::this_thread::yield();
        }

        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Produced: " << value << " by thread " << id << std::endl;
    }
}

void consumer(LockFreeQueue<int>& queue, int id) {
    int value;
    while (true) {
        if (queue.dequeue(value)) {
            total_consumed.fetch_add(1, std::memory_order_relaxed);

            {
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cout << "Consumed: " << value << " by thread " << id << std::endl;
            }

            if (total_consumed.load(std::memory_order_relaxed) >= TOTAL_ITEMS) break;
        } else {
            if (done.load(std::memory_order_relaxed)) break;
            std::this_thread::yield();
        }
    }
}

int main() {
    LockFreeQueue<int> queue(QUEUE_CAPACITY);

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // launch producers
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producers.emplace_back(producer, std::ref(queue), i);
    }

    // launch consumers
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumers.emplace_back(consumer, std::ref(queue), i);
    }

    // join producers
    for (auto& producer : producers) {
        producer.join();
    }

    // signal producers to stop
    done.store(true, std::memory_order_relaxed);

    // join consumers
    for (auto& consumer : consumers) {
        consumer.join();
    }

    std::cout << "Done Producing and Consuming" << std::endl;
    std::cout << "Total consumed: " << total_consumed.load() << std::endl;

    return 0;
    
    
}