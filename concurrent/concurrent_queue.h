#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace aria {

template <class T> class concurrent_queue {
public:
  concurrent_queue() = default;
  ~concurrent_queue() = default;
  concurrent_queue(const concurrent_queue &) = delete;
  concurrent_queue &operator=(const concurrent_queue &) = delete;
  concurrent_queue(const concurrent_queue &&) = delete;
  concurrent_queue &operator=(concurrent_queue &&) = delete;

  void push(const T &val) {
    std::unique_lock lg(m_mut);
    m_queue.push(val);
    m_cond.notify_one();
  }

  T pop() {
    std::unique_lock lk(m_mut);
    m_cond.wait(lk, [&]() { return !m_queue.empty(); });
    auto res = std::move(m_queue.front());
    m_queue.pop();
    m_cond.notify_one();
    return res;
  }

  std::optional<T> try_pop() {
    std::unique_lock lk(m_mut);
    if (!m_queue.empty()) {
      auto res = std::optional<T>(std::move(m_queue.front()));
      m_queue.pop();
      m_cond.notify_one();
      return res;
    }
    return {};
  }

  bool empty() const {
    std::lock_guard(m_mut);
    return m_queue.empty();
  }

  auto size() const {
    std::lock_guard(m_mut);
    return m_queue.size();
  }

private:
  std::queue<T> m_queue;
  std::condition_variable m_cond;
  mutable std::mutex m_mut;
};

} // namespace aria