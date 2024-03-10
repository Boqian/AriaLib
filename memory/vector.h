#pragma once

#include <cstring> //memcpy
#include "allocator.h"

namespace aria {

template <class T, class Allocator = allocator<T>>
class vector {
 public:
  using size_type = unsigned int;
  using value_type = T;
  using pointer = value_type*;

  vector() noexcept = default;


  vector(size_type n, const T& value = T()) {
    add_space(n);
    m_size += n;
    for (int i = 0; i < n; i++) {
      get(i) = value;
    }
  }

  void push_back(const T& value) {
    add_space(1);
    get(m_size) = value;
    m_size++;
  }

  T& operator[](size_type i) { return get(i); }

 private:
  Allocator m_alloc;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;

  T& get(size_type i) { return *(m_ptr + i); }

  void add_space(size_type n) {
    if (m_capacity >= m_size + n) return;
    auto new_capacity = std::max<size_type>(1, m_capacity); 
    while (new_capacity < m_size + n) new_capacity *= 2;
    auto new_ptr = m_alloc.allocate(new_capacity);
    memcpy(new_ptr, m_ptr, m_size * sizeof(T));
    m_alloc.deallocate(m_ptr, m_capacity);
    m_capacity = new_capacity;
    m_ptr = new_ptr;
  }

}; 

}  // namespace aria