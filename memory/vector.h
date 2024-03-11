#pragma once

#include <cstring> //memcpy
#include <stdexcept>
#include "allocator.h"
#include <initializer_list>

namespace aria {

template <class T, class... Args>
constexpr T* construct_at(T* p, Args&&... args) {
  return ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
}

template <class T, class Allocator = allocator<T>>
class vector {
 public:
  using size_type = unsigned int;
  using value_type = T;
  using pointer = value_type*;

  vector() noexcept = default;

  ~vector() { free_and_destruct_all(); }

  vector(std::initializer_list<T> init) {
    add_space(init.size());
    m_size += init.size();
    for (size_type i = 0; auto& value : init) {
      construct_at(get(i++), value);
    }
  }

  vector(size_type n, const T& value = T()) {
    add_space(n);
    m_size += n;
    for (int i = 0; i < n; i++) {
      construct_at(get(i), value);
    }
  }

  vector(const vector& rhs) {
    add_space(rhs.capacity());
    m_size = rhs.size();
    m_capacity = rhs.capacity();
    for (int i = 0; i < m_size; i++) {
      construct_at(get(i), rhs[i]);
    }
  }

  void push_back(const T& value) {
    add_space(1);
    *get(m_size) = value;
    m_size++;
  }

  void pop_back() {
    if (m_size > 0) m_size--;
  }

  void reserve(size_type new_cap) {

  }

  size_type size() const { return m_size; }
  size_type capacity() const { return m_capacity; }

  T& operator[](size_type i) { return *get(i); }
  const T& operator[](size_type i) const { return *get(i); }
  T& at(size_type i) {
    check_position(i);
    return *get(i);
  }
  const T& at(size_type i) const {
    check_position(i);
    return *get(i);
  }

  T* data() noexcept { return m_ptr; }
  const T* data() const noexcept { return m_ptr; }

 private:
  Allocator m_alloc;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;

  T* get(size_type i) { return m_ptr + i; }
  const T* get(size_type i) const { return m_ptr + i; }

  void check_position(size_type i) const {
    if (i >= m_size) throw std::out_of_range("");
  }

  void destruct(size_type i) { (m_ptr + i)->~T(); }

  void free_and_destruct_all() {
    for (int i = 0; i < m_size; i++) destruct(i);
    m_alloc.deallocate(m_ptr, m_capacity);
  }

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