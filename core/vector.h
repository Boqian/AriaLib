#pragma once

#include "allocator.h"
#include "exception.h"
#include "iterator.h"
#include "utility.h"

namespace aria {

template <class VectorType> class vec_const_iterator {
public:
  using value_type = typename VectorType::value_type;
  using pointer = typename VectorType::const_pointer;
  using reference = const value_type &;
  using difference_type = ptrdiff_t;

  vec_const_iterator() = default;
  vec_const_iterator(pointer p) : ptr(p) {}

  reference operator*() const noexcept { return *ptr; }
  pointer operator->() const noexcept { return ptr; }

  vec_const_iterator &operator++() noexcept {
    ++ptr;
    return *this;
  }
  vec_const_iterator operator++(int) noexcept {
    auto temp = *this;
    ++ptr;
    return temp;
  }
  vec_const_iterator &operator--() noexcept {
    --ptr;
    return *this;
  }
  vec_const_iterator operator--(int) noexcept {
    auto temp = *this;
    --ptr;
    return temp;
  }

  vec_const_iterator &operator+=(const difference_type d) noexcept {
    ptr += d;
    return *this;
  }

  vec_const_iterator &operator-=(const difference_type d) noexcept {
    ptr -= d;
    return *this;
  }

  vec_const_iterator operator+(const difference_type d) const noexcept {
    auto temp = *this;
    temp += d;
    return temp;
  }

  vec_const_iterator operator-(const difference_type d) const noexcept {
    auto temp = *this;
    temp -= d;
    return temp;
  }

  difference_type operator-(vec_const_iterator rhs) const noexcept { return ptr - rhs.ptr; }

  auto operator<=>(const vec_const_iterator &) const noexcept = default;

private:
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
  pointer operator->() const noexcept { return const_cast<pointer>(Base::operator->()); }

  vec_iterator &operator++() noexcept {
    Base::operator++();
    return *this;
  }
  vec_iterator operator++(int) noexcept {
    auto temp = *this;
    Base::operator++();
    return temp;
  }
  vec_iterator &operator--() noexcept {
    Base::operator--();
    return *this;
  }
  vec_iterator operator--(int) noexcept {
    auto temp = *this;
    Base::operator--();
    return temp;
  }

  vec_iterator &operator+=(const difference_type d) {
    Base::operator+=(d);
    return *this;
  }

  vec_iterator &operator-=(const difference_type d) {
    Base::operator-=(d);
    return *this;
  }

  vec_iterator operator+(const difference_type d) const {
    auto temp = *this;
    temp += d;
    return temp;
  }

  using Base::operator-;

  vec_iterator operator-(const difference_type d) const {
    auto temp = *this;
    temp -= d;
    return temp;
  }
};

template <class T, class Allocator = allocator<T>> class vector {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using allocator_type = Allocator;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using iterator = vec_iterator<vector<T, Allocator>>;
  using const_iterator = vec_const_iterator<vector<T, Allocator>>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  vector() noexcept = default;

  ~vector() { free_and_destruct_all(); }

  vector(initializer_list<T> init) {
    reserve(init.size());
    m_size = init.size();
    for (size_type i = 0; auto &value : init) {
      construct_at(get(i++), value);
    }
  }

  vector(size_type n, const T &value = T()) {
    reserve(n);
    m_size += n;
    for (int i = 0; i < n; i++) {
      construct_at(get(i), value);
    }
  }

  vector(const vector &rhs) {
    reserve(rhs.capacity());
    m_size = rhs.size();
    for (int i = 0; i < m_size; i++) {
      construct_at(get(i), rhs[i]);
    }
  }

  vector(vector &&rhs) noexcept { swap(rhs); }

  vector &operator=(const vector &rhs) {
    if (this != &rhs) {
      auto v = rhs;
      swap(v);
    }
    return *this;
  }

  vector &operator=(vector &&rhs) noexcept {
    if (this != &rhs) {
      auto v = move(rhs);
      swap(v);
    }
    return *this;
  }

  void push_back(const T &value) {
    reserve(new_capacity(1));
    construct_at(get(m_size), value);
    m_size++;
  }

  template <class... Args> void emplace_back(Args &&...args) {
    reserve(new_capacity(1));
    aria::construct_at(get(m_size), ::aria::forward<Args>(args)...);
    m_size++;
  }

  void pop_back() {
    if (m_size > 0) {
      m_size--;
      decstuct_at(m_size);
    }
  }

  void reserve(size_type new_cap) {
    if (new_cap <= capacity())
      return;
    auto new_ptr = m_alloc.allocate(new_cap);
    memcpy(new_ptr, m_ptr, m_size * sizeof(T));
    m_alloc.deallocate(m_ptr, m_capacity);
    m_capacity = new_cap;
    m_ptr = new_ptr;
  }

  size_type size() const noexcept { return m_size; }
  size_type capacity() const noexcept { return m_capacity; }
  bool empty() const noexcept { return m_size == 0; }
  reference back() noexcept { return *get(m_size - 1); }
  const_reference back() const noexcept { return *get(m_size - 1); }

  reference operator[](size_type i) { return *get(i); }
  const_reference operator[](size_type i) const { return *get(i); }
  reference at(size_type i) {
    check_position(i);
    return *get(i);
  }
  const_reference at(size_type i) const {
    check_position(i);
    return *get(i);
  }

  pointer data() noexcept { return m_ptr; }
  const_pointer data() const noexcept { return m_ptr; }

  void swap(vector &rhs) noexcept {
    aria::swap(m_size, rhs.m_size);
    aria::swap(m_capacity, rhs.m_capacity);
    aria::swap(m_ptr, rhs.m_ptr);
    aria::swap(m_alloc, rhs.m_alloc);
  }

  bool operator==(const vector &rhs) const noexcept {
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

  auto begin() const noexcept { return const_iterator(get(0)); }
  auto end() const noexcept { return const_iterator(get(m_size)); }
  auto begin() noexcept { return iterator(get(0)); }
  auto end() noexcept { return iterator(get(m_size)); }
  auto rbegin() const noexcept { return const_reverse_iterator(end()); }
  auto rend() const noexcept { return const_reverse_iterator(begin()); }
  auto rbegin() noexcept { return reverse_iterator(end()); }
  auto rend() noexcept { return reverse_iterator(begin()); }

private:
  Allocator m_alloc;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;

  pointer get(size_type i) { return m_ptr + i; }
  const_pointer get(size_type i) const { return m_ptr + i; }

  void check_position(size_type i) const {
    if (i >= m_size)
      throw std::out_of_range("");
  }

  void decstuct_at(size_type i) { destroy_at(get(i)); }

  void free_and_destruct_all() {
    for (int i = 0; i < m_size; i++)
      decstuct_at(i);
    m_alloc.deallocate(m_ptr, m_capacity);
  }

  size_type new_capacity(size_type add_size) {
    if (capacity() == 0)
      return add_size;
    auto min_capacity = m_size + add_size;
    auto new_capacity = capacity();
    while (new_capacity < min_capacity)
      new_capacity *= 2;
    return new_capacity;
  }
};

template <class T, class A> void swap(vector<T, A> &a, vector<T, A> &b) { a.swap(b); }

} // namespace aria