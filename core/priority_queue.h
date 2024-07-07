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
  explicit priority_queue(const Compare &compare) : m_cmp(compare) {}

  template <input_iterator It>
  priority_queue(It first, It last, const Compare &compare = Compare()) : m_cmp(compare), m_data(first, last) {}

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

  void swap(priority_queue &rhs) {
    using aria::swap;
    swap(m_data, rhs.m_data);
    swap(m_cmp, rhs.m_cmp);
  }

private:
  container_type m_data;
  value_compare m_cmp;
};

template <class T, class Container, class Compare>
void swap(priority_queue<T, Container, Compare> &lhs, priority_queue<T, Container, Compare> &rhs) {
  lhs.swap(rhs);
}

} // namespace aria