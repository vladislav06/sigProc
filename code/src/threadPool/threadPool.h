//
// Created by vm on 24.28.11.
//
#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>

/**
 * Global static thread pool
 */
class ThreadPool {
public:

    struct Job {
        std::function<void()> job;
        void *owner = nullptr;
        size_t jobId = 0;
        std::atomic_bool inProgress = false;
    };

    /**
     * Will return instance of thread pool
     * @return
     */
    static ThreadPool &get() {
        static ThreadPool instance;
        return instance;
    }

    /**
     * Start thread pool, mus be called only once
     */
    void start();

    /**
     * Queue job for thread pool to execute
     * @param job job to be executed
     * @param owner owner of this job
     */
    std::shared_ptr<Job> queueJob(const std::function<void()> &job, void *owner);

    std::vector<std::shared_ptr<ThreadPool::Job>> getJobs(void *owner);

    void deleteJob(std::shared_ptr<ThreadPool::Job>  job);

    /**
     * Will stop thread pool
     */
    void stop();

    /**
     * Returns whether thread pool has jobQueue to do
     * @return
     */
    bool busy();

    ~ThreadPool() {
        stop();
    }

private:
    explicit ThreadPool() = default;

    ThreadPool(ThreadPool const &);

    void operator=(ThreadPool const &);

    void ThreadLoop(int index);

    std::unique_ptr<ThreadPool> instance{};

    bool should_terminate = false;           // Tells threads to stop looking for jobQueue
    size_t nextJobId = 0;

    std::mutex threadMutex{};
    std::condition_variable mutex_condition{}; // Allows threads to wait on new jobQueue or termination
    std::vector<std::thread> threads{};

    std::mutex queueMutex{};                  // Prevents data races to the job queue
    std::deque<std::shared_ptr<Job>> jobQueue{};

    std::mutex jobsExecutedMutex{};
    std::vector<std::shared_ptr<Job>> jobsExecuted{};

};
