#pragma once
#include <atomic>
#include <memory>
#include <optional>

namespace aria {

template <class T> class spsc_queue {
public:
  using size_t = std::size_t;

  spsc_queue(size_t capacity) : m_maxsize(capacity + 1), m_buffer(std::make_unique<T[]>(capacity + 1)) {}

  bool push(const T &x) {
    const auto head = m_head.load();
    const auto tail = m_tail.load(), tail_next = increment(tail);
    if (tail_next == head)
      return false; // full
    (*m_buffer)[tail] = x;
    m_tail.store(tail_next);
    return true;
  }

  bool pop(T &item) {
    const auto head = m_head.load();
    const auto tail = m_tail.load();
    if (head == tail)
      return false; // empty;
    item = (*m_buffer)[head];
    m_head.store(increment(head));
    return true;
  }

  bool empty() const { return m_head.load() == m_tail.load(); }

  bool full() const { return (m_head.load() + 1) % m_maxsize == m_tail.load(); }

  const T &front() const { return (*m_buffer)[m_head.load()]; }

private:
  size_t increment(size_t pos) {
    if (++pos == m_maxsize)
      pos = 0;
    return pos;
  }

  alignas(64) std::atomic<size_t> m_head{};
  alignas(64) std::atomic<size_t> m_tail{};
  alignas(64) const std::unique_ptr<T[]> m_buffer;
  const std::size_t m_maxsize;
};

} // namespace aria