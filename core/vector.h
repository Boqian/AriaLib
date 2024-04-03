#pragma once

#include <cstring> //memcpy
#include <stdexcept>
#include "utility.h"
#include "allocator.h"

namespace aria {

template <class VectorType> class vec_const_iterator {
public:
    using value_type = typename VectorType::value_type;
    using pointer = typename VectorType::const_pointer;
    using reference = const value_type &;
    using difference_type = long long;

    vec_const_iterator() = default;
    vec_const_iterator(pointer p) : ptr(p) {}

    reference operator*() const noexcept { return *ptr; }
    reference operator->() const noexcept { return *ptr; }

    vec_const_iterator &operator++() {
      ++ptr;
      return *this;
    }
    vec_const_iterator operator++(int) {
      auto temp = *this;
      ++ptr;
      return temp;
    }
    vec_const_iterator &operator--() {
      --ptr;
      return *this;
    }
    vec_const_iterator operator--(int) {
      auto temp = *this;
      --ptr;
      return temp;
    }

    vec_const_iterator &operator+=(difference_type d) {
      ptr += d;
      return *this;
    }

    vec_const_iterator &operator-=(difference_type d) {
      ptr -= d;
      return *this;
    }



    auto operator<=>(const vec_const_iterator &) const noexcept = default;

protected:
    pointer ptr = nullptr;
};

 template <class VectorType> class vec_iterator : public vec_const_iterator<VectorType> {
     using Base = vec_const_iterator<VectorType>;
 public:
     using value_type = typename VectorType::value_type;
     using pointer = typename VectorType::pointer;
     using reference = value_type &;
     using difference_type = typename Base::difference_type;
     using Base::Base;

     reference operator*() const noexcept { return const_cast<reference>(Base::operator*()); }
     reference operator->() const noexcept { return const_cast<reference>(Base::operator->()); }
 };

template <class T, class Allocator = allocator<T>>
class vector {
 public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = vec_iterator<vector<T,Allocator>>;
  using const_iterator = vec_const_iterator<vector<T,Allocator>>;
 
  vector() noexcept = default;

  ~vector() { free_and_destruct_all(); }

  vector(std::initializer_list<T> init) {
    reserve(init.size());
    m_size = init.size();
    for (size_type i = 0; auto& value : init) {
      construct_at(get(i++), value);
    }
  }

  vector(size_type n, const T& value = T()) {
    reserve(n);
    m_size += n;
    for (int i = 0; i < n; i++) {
      construct_at(get(i), value);
    }
  }

  vector(const vector& rhs) {
    reserve(rhs.capacity());
    m_size = rhs.size();
    for (int i = 0; i < m_size; i++) {
      construct_at(get(i), rhs[i]);
    }
  }

  vector(vector&& rhs) noexcept {
    swap(rhs);
    vector().swap(rhs);
  }

  vector& operator=(const vector& rhs) {
    if (this != &rhs) {
      auto v = rhs;
      swap(v);
    }
    return *this;
  }

  vector& operator=(vector&& rhs) noexcept {
    if (this != &rhs) {
      auto v = move(rhs);
      swap(v);
    }
    return *this;
  }

  void push_back(const T& value) {
    reserve(new_capacity(1));
    construct_at(get(m_size), value);
    m_size++;
  }

  template<class... Args>
  void emplace_back(Args&& ... args) {
    reserve(new_capacity(1));
    aria::construct_at(get(m_size), forward<Args>(args)...);
    m_size++;
  }

  void pop_back() {
    if (m_size > 0) {
      m_size--;
      decstuct_at(m_size);
    }
  }

  void reserve(size_type new_cap) {
    if (new_cap <= capacity()) return;
    auto new_ptr = m_alloc.allocate(new_cap);
    memcpy(new_ptr, m_ptr, m_size * sizeof(T));
    m_alloc.deallocate(m_ptr, m_capacity);
    m_capacity = new_cap;
    m_ptr = new_ptr;
  }

  size_type size() const noexcept { return m_size; }
  size_type capacity() const noexcept { return m_capacity; }
  bool empty() const noexcept { return m_size == 0; }
  T& back() noexcept { return *get(m_size - 1); }
  const T& back() const noexcept { return *get(m_size - 1); }

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

  void swap(vector& rhs) noexcept {
    aria::swap(m_size, rhs.m_size);
    aria::swap(m_capacity, rhs.m_capacity);
    aria::swap(m_ptr, rhs.m_ptr);
    aria::swap(m_alloc, rhs.m_alloc);
  }

  bool operator==(const vector& rhs) const noexcept {
    if (this == &rhs) return true;
    if (size() != rhs.size()) return false;
    for (size_type i = 0; i < size(); i++) {
      if ((*this)[i] != rhs[i]) return false;
    }
    return true;
  }

  const_iterator begin() const noexcept { return const_iterator(get(0)); }
  const_iterator end() const noexcept { return const_iterator(get(m_size)); }
  iterator begin() noexcept { return iterator(get(0)); }
  iterator end() noexcept { return iterator(get(m_size)); } 

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

  void decstuct_at(size_type i) { destroy_at(get(i)); }

  void free_and_destruct_all() {
    for (int i = 0; i < m_size; i++) decstuct_at(i);
    m_alloc.deallocate(m_ptr, m_capacity);
  }

  size_type new_capacity(size_type add_size) { 
      if (capacity() == 0) return add_size;
      auto min_capacity = m_size + add_size;
      auto new_capacity = capacity();
      while (new_capacity < min_capacity) new_capacity *= 2;
      return new_capacity;
  }
}; 

}  // namespace aria