#pragma once
#include <cstddef>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable cond;
        bool stop;
        size_t num_threads;

    public:
        ThreadPool(size_t num_threads) : num_threads(num_threads), stop(false) {
            for (size_t i = 0; i < num_threads; i++) {
                threads.emplace_back([this] {
                    while (true) {
                        // initialize a task
                        std::function<void()> task;
                        {
                            // lock the queue for thread safety
                            std::unique_lock<std::mutex> lock(queue_mutex);
                            
                            // wait until there is a task or the threadpool is stopped
                            cond.wait(lock, [this] { return !tasks.empty() || stop;} );

                            // if the threadpool is stopped, break the loop
                            if (stop && tasks.empty()) {
                                return;
                            }

                            // get the task from queue and execute it
                            task = std::move(tasks.front());
                            tasks.pop();
                        }

                        task();
                    }
                });
            }
        };

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
            // get the return type of the function
            using return_type = typename std::invoke_result<F, Args...>::type;

            // package the function and arguments into a function object and make it a shared pointer
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            // get the result of the task as a future
            std::future<return_type> res = task->get_future();

            // lock queue for thread safety
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.emplace([task]() { (*task)(); });
            }

            // notify a thread that there is a new task
            cond.notify_one();
            return res;
        }
       
        ~ThreadPool(){
            // stop all threads
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true;
            }
            cond.notify_all();

            // join all threads
            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        };
};