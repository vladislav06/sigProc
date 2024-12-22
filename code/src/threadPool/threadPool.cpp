//
// Created by vm on 24.18.6.
//

#include "threadPool.h"

void ThreadPool::start() {
    const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    for (uint32_t ii = 0; ii < num_threads; ++ii) {
        jobsExecuted.push_back(nullptr);
        threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this, ii));
    }
}

void ThreadPool::ThreadLoop(int index) {
    while (true) {
        std::shared_ptr<Job> job;
        {
            std::unique_lock<std::mutex> lockTemp(threadMutex);
            mutex_condition.wait(lockTemp, [this] {
                return !jobQueue.empty() || should_terminate;
            });
            if (should_terminate) {
                return;
            }
        }
        {
            std::scoped_lock lock(queueMutex, jobsExecutedMutex);
            if (jobQueue.empty()) {
                continue;
            }
            job = jobQueue.front();
            job->inProgress = true;
            jobQueue.pop_front();
            jobsExecuted[index] = job;
            job->inProgress = true;
        }
        job->job();
        {
            std::lock_guard<std::mutex> lock(jobsExecutedMutex);
            job->inProgress = false;
            jobsExecuted[index] = nullptr;
        }

    }
}

std::shared_ptr<ThreadPool::Job> ThreadPool::queueJob(const std::function<void()> &job, void *owner) {
    std::shared_ptr<Job> j = std::make_shared<Job>();
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        j->job = job;
        j->jobId = nextJobId;
        nextJobId++;
        j->owner = owner;
        jobQueue.push_back(std::move(j));
    }
    mutex_condition.notify_one();
    return j;
}

bool ThreadPool::busy() {
    bool poolbusy;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        poolbusy = !jobQueue.empty();
    }
    return poolbusy;
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread: threads) {
        active_thread.join();
    }
    threads.clear();
}

std::vector<std::shared_ptr<ThreadPool::Job>> ThreadPool::getJobs(void *owner) {
    std::vector<std::shared_ptr<ThreadPool::Job>> ownerJobs;
    {
        std::scoped_lock lock(queueMutex, jobsExecutedMutex);


        //push jobQueue in queue
        for (auto &job: jobQueue) {
            if (job->owner == owner) {
                ownerJobs.push_back(job);
            }
        }

        //push jobQueue currently executed
        for (auto &job: jobsExecuted) {
            if (job == nullptr) {
                continue;
            }
            if (job->owner == owner) {
                ownerJobs.push_back(job);
            }
        }
    }
    return ownerJobs;
}

void ThreadPool::deleteJob(std::shared_ptr<ThreadPool::Job> job) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        erase_if(jobQueue, [&job](std::shared_ptr<Job> &j) { return j->jobId == job->jobId; });
    }
}
