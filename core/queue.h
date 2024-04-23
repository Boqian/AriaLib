#pragma once

#include "deque.h"

namespace aria {
template <class T, class Container = deque<T>> class queue {
public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = Container::size_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;

  queue() = default;

  void push(const_reference val) { m_container.push_back(val); }
  void pop() { m_container.pop_front(); }
  reference front() { return m_container.front(); }
  const_reference front() const { return m_container.front(); }
  reference back() { return m_container.back(); }
  const_reference back() const { return m_container.back(); }

  bool empty() const noexcept { return m_container.empty(); }
  size_type size() const noexcept { return m_container.size(); }

  void swap(queue &rhs) noexcept { swap(m_container, rhs.m_container); }

private:
  container_type m_container;
};

template <class T, class Container> void swap(queue<T, Container> &a, queue<T, Container> &b) { a.swap(b); }

} // namespace aria