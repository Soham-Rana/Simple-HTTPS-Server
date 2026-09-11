#include "thread/thread_pool.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace thread {

ThreadPool::ThreadPool(std::size_t worker_count) {
    const auto count = worker_count == 0 ? std::max<std::size_t>(1, std::thread::hardware_concurrency()) : worker_count;
    workers_.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        workers_.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::shutdown() {
    {
        std::lock_guard lock(mutex_);
        accepting_ = false;
        stopping_ = true;
    }
    condition_.notify_all();
}

void ThreadPool::wait() {
    shutdown();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

std::size_t ThreadPool::worker_count() const noexcept { return workers_.size(); }

std::size_t ThreadPool::pending_tasks() const {
    std::lock_guard lock(mutex_);
    return tasks_.size();
}

bool ThreadPool::accepting_tasks() const noexcept { return accepting_; }

void ThreadPool::worker_loop() {
    for (;;) {
        Task task;
        {
            std::unique_lock lock(mutex_);
            condition_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });

            if (tasks_.empty()) {
                if (stopping_) {
                    return;
                }
                continue;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        if (task) {
            task();
        }
    }
}

}  // namespace thread
