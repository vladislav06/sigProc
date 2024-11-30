//
// Created by vm on 24.28.11.
//
#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

/**
 * Global static thread pool
 */
class ThreadPool {
public:
//    static ThreadPool &operator()() {
//        static ThreadPool instance;
//        return instance;
//    }
    static ThreadPool &get() {
        static ThreadPool instance;
        return instance;
    }

    void start();

    void queueJob(const std::function<void()> &job);

    void stop();

    bool busy();

    ~ThreadPool(){
        stop();
    }

private:
    explicit ThreadPool() = default;

    ThreadPool(ThreadPool const &);

    void operator=(ThreadPool const &);

    void ThreadLoop();

    std::unique_ptr<ThreadPool> instance{};

    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex{};                  // Prevents data races to the job queue
    std::condition_variable mutex_condition{}; // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads{};
    std::queue<std::function<void()>> jobs{};
};
