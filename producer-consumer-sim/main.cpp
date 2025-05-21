#include "BoundedBuffer.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>

int BUFFER_SIZE = 1000;
int PRODUCER_COUNT = 10000;

std::mutex cout_mutex;

void producer(BoundedBuffer<int>& buffer) {
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        buffer.push(i);
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Produced: " << i << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    buffer.stop();
}

void consumer(BoundedBuffer<int>& buffer) {
    int item;
    while (buffer.pop(item)) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Consumed: " << item << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "Consumer stopped" << std::endl;
}

int main() {
    BoundedBuffer<int> buffer(BUFFER_SIZE);

    std::thread producer_thread(producer, std::ref(buffer));
    std::thread consumer_thread(consumer, std::ref(buffer));

    producer_thread.join();
    consumer_thread.join();

    return 0;
}
