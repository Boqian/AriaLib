#pragma once

#include <cstring> //strlen memcpy
#include "allocator.h"

namespace aria {

 template <class CharT, class Allocator = allocator<CharT>> class basic_string {
public:
  using size_type = size_t;
  using value_type = CharT;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;

  basic_string() = default;
  ~basic_string() noexcept { reset(); }

  basic_string(const_pointer str) : basic_string(str, strlen(str)) {}
  basic_string(const basic_string &rhs) : basic_string(rhs.m_ptr, rhs.size(), rhs.capacity()) {}

  void swap(basic_string &rhs) noexcept {
    ::aria::swap(m_alloc, rhs.m_alloc);
    ::aria::swap(m_ptr, rhs.m_ptr);
    ::aria::swap(m_size, rhs.m_size);
    ::aria::swap(m_capacity, rhs.m_capacity);
  }

  size_type size() const noexcept { return m_size; }
  size_type capacity() const noexcept { return m_capacity; }
  bool empty() const noexcept { return m_size == 0; }
  reference back() noexcept { return *get(m_size - 1); }
  const_reference back() const noexcept { return *get(m_size - 1); }
  reference operator[](size_type i) { return *get(i); }
  const_reference &operator[](size_type i) const { return *get(i); }

  void reserve(size_type new_cap) {
    if (new_cap <= capacity())
      return;
    auto new_ptr = m_alloc.allocate(new_cap);
    memcpy(new_ptr, m_ptr, m_size * sizeof(value_type));
    reset();
    m_capacity = new_cap;
    m_ptr = new_ptr;
  }

  bool operator==(const basic_string &rhs) const noexcept {
    if (this == &rhs)
      return true;
    if (size() != rhs.size())
      return false;
    for (size_type i = 0; i < size(); i++) {
      if ((*this)[i] != rhs[i])
        return false;
    }
    return true;
  }

  void clear() noexcept { m_size = 0; }


private:
  pointer get(size_type i) { return m_ptr + i; }
  const pointer get(size_type i) const { return m_ptr + i; }

  basic_string(const_pointer str, size_type size, size_type cap = 0) {
    if (cap == 0)
      cap = size;
    reserve(cap);
    copy(str, size, cap);
  }

  void reset() noexcept {
    if (m_ptr) {
      m_alloc.deallocate(m_ptr, m_capacity);
      m_size = 0;
      m_capacity = 0;
    }
  }
  
  void copy(const_pointer src, size_type size, size_type cap) {
    if (src) {
      memcpy(m_ptr, src, size * sizeof(value_type));
      m_size = size;
      m_capacity = cap;
    }
  }

  Allocator m_alloc;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;
};

using string = basic_string<char>;

} // namespace aria
