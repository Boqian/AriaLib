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

  void pause() { m_paused = true; }

  void unpause() { m_paused = false; }

  bool is_running() const { return !m_stopped; }

  template <typename F>
  void detach_task(F&& task) {
    auto lock = m_tasks.wlock();
    if (!m_stopped) lock->push(std::move(task));
  }

  void purge() {
    auto lock = m_tasks.wlock();
    while (!lock->empty()) lock->pop();
  }

 private:
  using TTask = std::function<void()>;

  void worker() {
    while (!m_stopped) {
      auto lock = m_tasks.wlock();
      m_tasks_cv.wait(lock.as_lock(), [&]() { return m_stopped || (!m_paused && !lock->empty()); });
      if (m_stopped) break;
      auto task{std::move(lock->front())};
      lock->pop();
      lock.unlock();
      m_tasks_cv.notify_one();
      task();
    }
  }

  size_t m_size;
  Synchronized<std::queue<TTask>> m_tasks;
  std::vector<std::jthread> m_pool;
  std::atomic_bool m_stopped = true;
  std::atomic_bool m_paused = false;
  std::condition_variable_any m_tasks_cv;
};

}  // namespace aria