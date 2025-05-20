#include "ThreadPool.h"
#include <iostream>
#include <chrono>

int main() {
    ThreadPool pool(4);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 100; ++i) {
        futures.push_back(pool.enqueue([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "Running task " << i << " on thread " << std::this_thread::get_id() << std::endl;
            return i * i;
        }));
    }

    for (auto& fut : futures) {
        std::cout << fut.get() << " ";
    }
    
    return 0;
}