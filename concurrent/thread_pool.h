#include <thread>
#include <vector>
#include <memory>
#include <future>
#include <functional>
#include <queue>
#include <mutex>

namespace aria {

class ThreadPool {
 public:
  explicit ThreadPool(size_t n) : m_size(n) {}

  void start() {

  }

  void pause() { is_running = false; }

  void unpause() { is_running = true; }

  bool is_paused() const { return is_running; }
       
  template <typename F>
  void detach_task(F&& task) {
    std::lock_guard lg(m_task_mutex);
    m_tasks.emplace(task);
  }

 private:
  using TTask = std::function<void()>;

  void process() {
    while (is_running) {

    }
  }

  size_t m_size;
  std::queue<TTask> m_tasks;
  std::vector<std::thread> m_pool;
  std::mutex m_task_mutex;
  std::atomic_bool is_running = false;
};

}