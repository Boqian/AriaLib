#pragma once

#include "deque.h"

namespace aria {
template <class T, class Container = deque<T>> class stack {
public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = Container::size_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;

  stack() = default;

  void push(const_reference val) { m_container.push_back(val); }
  void pop() { m_container.pop_back(); }
  reference top() { return m_container.back(); }
  const_reference top() const { return m_container.back(); }

  bool empty() const noexcept { return m_container.empty(); }
  size_type size() const noexcept { return m_container.size(); }

  void swap(stack &rhs) noexcept { swap(m_container, rhs.m_container); }

  bool operator==(stack &rhs) const noexcept { return m_container == rhs.m_container; }

private:
  container_type m_container;
};

template <class T, class Container> void swap(stack<T, Container> &a, stack<T, Container> &b) { a.swap(b); }

} // namespace aria