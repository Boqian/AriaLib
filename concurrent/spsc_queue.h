#pragma once
#include <atomic>
#include <memory>
#include <optional>

namespace aria {

template <class T> class spsc_queue {
public:
  using size_t = std::size_t;

  spsc_queue(size_t capacity) : m_max_size(capacity + 1), m_buffer(std::make_unique<T[]>(capacity + 1)) {}

  bool push(const T &x) {
    const auto r = m_read_pos.load(std::memory_order_acquire);
    const auto w = m_write_pos.load(std::memory_order_relaxed), next_w = increment(w);
    if (next_w == r)
      return false; // full
    (*m_buffer)[w] = x;
    m_write_pos.store(next_w, std::memory_order_release);
    return true;
  }

  bool pop(T &item) {
    const auto r = m_read_pos.load(std::memory_order_relaxed);
    const auto w = m_write_pos.load(std::memory_order_acquire);
    if (r == w)
      return false; // empty;
    item = (*m_buffer)[r];
    m_read_pos.store(increment(r), std::memory_order_release);
    return true;
  }

  bool empty() const { return m_read_pos.load() == m_write_pos.load(); }

  bool full() const { return increment(m_write_pos.load()) == m_read_pos.load(); }

  const T &front() const { return (*m_buffer)[m_read_pos.load()]; }

private:
  size_t increment(size_t pos) {
    if (++pos == m_max_size)
      pos = 0;
    return pos;
  }

  alignas(64) std::atomic<size_t> m_read_pos{};
  alignas(64) std::atomic<size_t> m_write_pos{};
  alignas(64) const std::unique_ptr<T[]> m_buffer;
  const std::size_t m_max_size;
};

} // namespace aria