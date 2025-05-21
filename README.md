# Learning C++ Threading

This repository documents my journey learning concurrent programming in C++. I implemented these thread-safe data structures to understand modern C++ threading concepts, lock-free algorithms, and thread pool patterns.

## What I Learned

### Producer-Consumer Simulation
A classic concurrency pattern implementation using condition variables. Key learnings:
- Condition variables for thread synchronization
- Mutex-based thread-safe queues
- Proper thread signaling and notification
- Thread-safe console output
- Graceful shutdown of concurrent operations

### Lock-Free Queue
My first attempt at implementing a lock-free data structure. Key learnings:
- How atomic operations work in C++
- Memory ordering and its impact on performance
- The ABA problem and how to prevent it
- Writing thread-safe code without mutexes
- The importance of proper initialization in concurrent code

### Thread Pool
A practical implementation that taught me:
- Modern C++ threading patterns
- Template metaprogramming with variadic templates
- Future/promise pattern for async operations
- Exception safety in concurrent code
- Resource management and graceful shutdown

## Code Examples

### Producer-Consumer
```cpp
// Classic producer-consumer pattern with condition variables
BoundedBuffer<int> buffer(1000);  // Thread-safe bounded buffer
std::thread producer(producer, std::ref(buffer));  // Producer thread
std::thread consumer(consumer, std::ref(buffer));  // Consumer thread
// Automatic synchronization using condition variables
```

### Lock-Free Queue
```cpp
// My first lock-free queue implementation
LockFreeQueue<int> queue(1024);  // Bounded buffer to prevent unbounded growth
queue.enqueue(42);               // Thread-safe without locks!
int value;
queue.dequeue(value);           // Safe concurrent access
```

### Thread Pool
```cpp
// A modern thread pool with futures
ThreadPool pool(4);  // Reuse threads instead of creating new ones
auto future = pool.enqueue([](int x) { return x * x; }, 42);
int result = future.get();  // Get results asynchronously
```

## Building
```bash
mkdir build && cd build
cmake ..
make
```

## Requirements
- C++17 or later (for std::invoke_result and other modern features)
- CMake 3.10+
- A C++ compiler with atomic operation support

## Next Steps
I'm planning to:
- Add more thread-safe data structures
- Implement performance benchmarks
- Add unit tests
- Explore more lock-free algorithms
- Add more concurrent programming patterns
