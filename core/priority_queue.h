#pragma once

#include "algorithm.h"
#include "vector.h"

namespace aria {

template <class T, class Container = vector<T>, class Compare = less<typename Container::value_type>> class priority_queue {
public:
  using container_type = Container;
  using value_compare = Compare;
  using value_type = Container::value_type;
  using size_type = Container::size_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;

  priority_queue() = default;

  template <class... Args> void emplace(Args &&...args) {
    m_data.emplace_back(forward<Args>(args)...);
    push_heap(begin(m_data), end(m_data), m_cmp);
  }

  void push(const value_type &value) { emplace(value); }
  void push(value_type &&value) { emplace(move(value)); }
  const_reference top() const { return m_data[0]; }
  bool empty() const { return m_data.empty(); }
  size_type size() const { return m_data.size(); }

  void pop() {
    pop_heap(begin(m_data), end(m_data));
    m_data.pop_back();
  }

private:
  container_type m_data;
  value_compare m_cmp;
};

} // namespace aria