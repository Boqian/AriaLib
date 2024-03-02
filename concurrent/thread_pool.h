#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "synchronized.h"

namespace aria {

/*
reference: https://github.com/bshoshany/thread-pool/blob/master/README.md
*/

class ThreadPool {
 public:
  explicit ThreadPool(size_t n) : m_size(n) {}

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void start() {
    for (int i = 0; i < m_size; i++) {
      m_pool.emplace_back([&] { worker(); });
    }

    m_stopped = false;
  }

  //wait for running task done
  void wait() {

  }

  void stop() {
    m_stopped = true;
    m_tasks_cv.notify_all();
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

  template <typename F>
  void detach_task(F&& task) {
    std::lock_guard lg(tasks_mutex);
    if (!m_stopped) m_tasks.push(std::move(task));
  }

  void purge() {
    std::lock_guard lg(tasks_mutex);
    while (!m_tasks.empty()) m_tasks.pop();
  }

 private:
  using TTask = std::function<void()>;

  void worker() {    
    std::unique_lock lock(tasks_mutex);
    while (true) {
      m_tasks_cv.wait(lock, [&]() { return m_stopped || (!m_paused && !m_tasks.empty()); });
      if (m_stopped) break;
      auto task{std::move(m_tasks.front())};
      m_tasks.pop();
      lock.unlock();
      m_tasks_cv.notify_all();
      task();
      lock.lock();
    }
  }

  size_t m_size;
  std::queue<TTask> m_tasks;
  std::vector<std::jthread> m_pool;
  std::atomic_bool m_stopped = true;
  std::atomic_bool m_paused = false;
  std::condition_variable_any m_tasks_cv;
  std::mutex tasks_mutex;
};

}  // namespace aria