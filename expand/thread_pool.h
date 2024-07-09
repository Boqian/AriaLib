#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "synchronized.h"
#include <iostream>

namespace aria {

/*
reference: https://github.com/bshoshany/thread-pool/blob/master/README.md
*/

class ThreadPool {
public:
  explicit ThreadPool(size_t n) : m_size(n) {}

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  void start() {
    std::lock_guard lg(tasks_mutex);
    m_stopped = false;
    for (int i = 0; i < m_size; i++) {
      m_pool.emplace_back([&] { worker(); });
    }
    m_tasks_cv.notify_all();
  }

  void stop() {
    std::lock_guard lg(tasks_mutex);
    if (!m_stopped) {
      m_stopped = true;
      m_tasks_cv.notify_all();
    }
  }

  void pause() {
    std::lock_guard lg(tasks_mutex);
    m_paused = true;
  }

  void unpause() {
    std::lock_guard lg(tasks_mutex);
    m_paused = false;
  }

  bool is_running() const { return !m_stopped; }

  template <typename F> void detach_task(F &&task) {
    std::lock_guard lg(tasks_mutex);
    m_tasks.push(std::forward<F>(task));
    m_tasks_cv.notify_one();
  }

  template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>> [[nodiscard]] std::future<R> submit_task(F &&task) {
    auto task_promise = std::make_shared<std::promise<R>>();
    detach_task([task = std::forward<F>(task), task_promise] {
      try {
        if constexpr (std::is_void_v<R>) {
          task();
          task_promise->set_value();
        } else {
          task_promise->set_value(task());
        }
      } catch (...) {
        try {
          task_promise->set_exception(std::current_exception());
        } catch (...) {
        }
      }
    });
    return task_promise->get_future();
  }

  void purge() {
    std::lock_guard lg(tasks_mutex);
    while (!m_tasks.empty())
      m_tasks.pop();
  }

private:
  using TTask = std::function<void()>;

  void worker() {
    std::unique_lock lock(tasks_mutex);
    m_num_active_threads++;
    while (true) {
      m_tasks_cv.wait(lock, [&]() { return m_stopped || (!m_paused && !m_tasks.empty()); });
      if (m_stopped)
        break;
      auto task{std::move(m_tasks.front())};
      m_tasks.pop();
      m_num_running_tasks++;
      m_tasks_cv.notify_one();
      lock.unlock();
      task();
      lock.lock();
      m_num_running_tasks--;
    }
    m_num_active_threads--;
  }

  size_t m_size;
  size_t m_num_running_tasks = 0;
  size_t m_num_active_threads = 0;
  std::queue<TTask> m_tasks;
  std::atomic_bool m_stopped = true;
  std::atomic_bool m_paused = false;
  std::condition_variable_any m_tasks_cv;
  std::mutex tasks_mutex;
  std::vector<std::jthread> m_pool;
};

} // namespace aria