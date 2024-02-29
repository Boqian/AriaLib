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

  void start() {
    for (int i = 0; i < m_size; i++) {
      m_pool.emplace_back([&] { deque_task(); });
    }

    m_is_running = true;
  }

  void pause() { m_is_running = false; }

  void unpause() { m_is_running = true; }

  bool is_paused() const { return m_is_running; }

  template <typename F>
  void detach_task(F&& task) {
    auto lock = m_tasks.wlock();
    if (m_is_running) lock->push(std::move(task));
  }

 private:
  using TTask = std::function<void()>;

  void deque_task() {
    while (m_is_running) {
      auto lock = m_tasks.wlock();
      m_cv.wait(lock.as_lock(), [&]() { return !lock->empty(); });
      auto task = std::move(lock->front());
      lock->pop();
      lock.unlock();
      m_cv.notify_all();
      task();
    }
  }

  size_t m_size;
  Synchronized<std::queue<TTask>> m_tasks;
  std::vector<std::thread> m_pool;
  std::atomic_bool m_is_running = false;
  std::condition_variable_any m_cv;
};

}  // namespace aria