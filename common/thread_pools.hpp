//
//  thread_pools.hpp
//  CPP_Consol
//
//  Created by admin on 26.12.2024.
//

#ifndef thread_pools_hpp
#define thread_pools_hpp

// Class that represents a simple thread pool
// Class that represents a simple thread pool

#include <thread>
#include <mutex>
#include <functional>

class ThreadPool
{
public:
    // // Constructor to creates a thread pool with given
    // number of threads
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());

    // Destructor to stop the thread pool
    ~ThreadPool();

    // Enqueue task for execution by the thread pool
    void enqueue(std::function<void()> task);

private:
    // Vector to store worker threads
    std::vector<std::thread> threads_;

    // Queue of tasks
    std::queue<std::function<void()>> tasks_;

    // Mutex to synchronize access to shared data
    std::mutex queue_mutex_;

    // Condition variable to signal changes in the state of
    // the tasks queue
    std::condition_variable cv_;

    // Flag to indicate whether the thread pool should stop
    // or not
    bool stop_ = false;
};

#endif /* thread_pools_hpp */
