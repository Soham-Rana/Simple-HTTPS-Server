#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace thread {

/**
 * Fixed-size worker thread pool.
 *
 * Tasks submitted to the pool are placed into a synchronized queue and
 * executed by available worker threads.
 */
class ThreadPool {
public:
    /**
     * Construct a thread pool with the requested number of workers.
     * If worker_count is zero, the implementation uses
     * std::thread::hardware_concurrency(), falling back to one worker.
     */
    explicit ThreadPool(std::size_t worker_count = 0);

    /**
     * Stop the worker threads and wait for them to finish.
     */
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /**
     * Submit a callable for asynchronous execution.
     *
     * The returned future contains the callable's result or exception.
     *
     * @throws std::runtime_error if the pool is shutting down.
     */
    template <typename F, typename... Args>
    auto submit(F&& function, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    /**
     * Request shutdown.
     *
     * New tasks are rejected. Tasks already queued are allowed to finish.
     */
    void shutdown();

    /**
     * Immediately prevent new tasks from being accepted and wait for
     * currently running/queued tasks to finish.
     */
    void wait();

    /**
     * Returns the number of worker threads.
     */
    [[nodiscard]]
    std::size_t worker_count() const noexcept;

    /**
     * Returns the approximate number of queued tasks.
     */
    [[nodiscard]]
    std::size_t pending_tasks() const;

    /**
     * Returns true if the pool is accepting new tasks.
     */
    [[nodiscard]]
    bool accepting_tasks() const noexcept;

private:
    using Task = std::function<void()>;

    /**
     * Worker thread entry point.
     */
    void worker_loop();

    /**
     * Worker threads.
     */
    std::vector<std::thread> workers_;

    /**
     * Pending task queue.
     */
    std::queue<Task> tasks_;

    /**
     * Synchronizes access to the task queue and state.
     */
    mutable std::mutex mutex_;

    /**
     * Wakes workers when tasks become available or shutdown begins.
     */
    std::condition_variable condition_;

    /**
     * Indicates whether new tasks may be submitted.
     */
    bool accepting_{true};

    /**
     * Indicates that shutdown has been requested.
     */
    bool stopping_{false};
};

// -----------------------------------------------------------------------------
// Template implementation
// -----------------------------------------------------------------------------

template <typename F, typename... Args>
auto ThreadPool::submit(F&& function, Args&&... args)
-> std::future<std::invoke_result_t<F, Args...>>
{
using ReturnType = std::invoke_result_t<F, Args...>;

auto task = std::make_shared<
    std::packaged_task<ReturnType()>
>(
    std::bind(
        std::forward<F>(function),
        std::forward<Args>(args)...
    )
);

std::future<ReturnType> result = task->get_future();

{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!accepting_) {
        throw std::runtime_error(
            "Cannot submit task: thread pool is shutting down"
        );
    }

    tasks_.emplace(
        [task]() {
            (*task)();
        }
    );
}

condition_.notify_one();

return result;


}

} // namespace thread