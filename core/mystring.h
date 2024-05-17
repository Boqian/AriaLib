#pragma once

#include "algorithm.h"
#include "allocator.h"
#include "iterator.h"
#include "utility.h"
#include <stdexcept>

namespace aria {

template <class StringType> class string_const_iterator {
public:
  using value_type = typename StringType::value_type;
  using pointer = typename StringType::const_pointer;
  using reference = const value_type &;
  using difference_type = ptrdiff_t;

  string_const_iterator() = default;
  string_const_iterator(pointer p) : ptr(p) {}

  reference operator*() const noexcept { return *ptr; }
  pointer operator->() const noexcept { return ptr; }

  string_const_iterator &operator++() {
    ++ptr;
    return *this;
  }

  string_const_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  string_const_iterator &operator--() {
    --ptr;
    return *this;
  }

  string_const_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  string_const_iterator &operator+=(const difference_type d) noexcept {
    ptr += d;
    return *this;
  }

  string_const_iterator &operator-=(const difference_type d) noexcept {
    ptr -= d;
    return *this;
  }

  string_const_iterator operator+(const difference_type d) const noexcept { return string_const_iterator(*this) += d; }
  string_const_iterator operator-(const difference_type d) const noexcept { return string_const_iterator(*this) -= d; }

  difference_type operator-(string_const_iterator rhs) const noexcept { return ptr - rhs.ptr; }

  auto operator<=>(const string_const_iterator &) const noexcept = default;

private:
  pointer ptr{};
};

template <class StringType> class string_iterator : public string_const_iterator<StringType> {
  using Base = string_const_iterator<StringType>;

public:
  using value_type = typename StringType::value_type;
  using pointer = typename StringType::pointer;
  using reference = value_type &;
  using difference_type = typename Base::difference_type;
  using Base::Base;

  string_iterator &operator++() noexcept {
    Base::operator++();
    return *this;
  }
  string_iterator operator++(int) noexcept {
    auto temp = *this;
    Base::operator++();
    return temp;
  }
  string_iterator &operator--() noexcept {
    Base::operator--();
    return *this;
  }
  string_iterator operator--(int) noexcept {
    auto temp = *this;
    Base::operator--();
    return temp;
  }

  string_iterator &operator+=(const difference_type d) {
    Base::operator+=(d);
    return *this;
  }

  string_iterator &operator-=(const difference_type d) {
    Base::operator-=(d);
    return *this;
  }

  string_iterator operator+(const difference_type d) const {
    auto temp = *this;
    temp += d;
    return temp;
  }

  using Base::operator-;

  string_iterator operator-(const difference_type d) const {
    auto temp = *this;
    temp -= d;
    return temp;
  }
};

template <class CharT, class Allocator = allocator<CharT>> class basic_string {
public:
  using size_type = size_t;
  using value_type = CharT;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  inline static const size_type npos = -1;
  using iterator = string_iterator<basic_string<CharT, Allocator>>;
  using const_iterator = string_const_iterator<basic_string<CharT, Allocator>>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  basic_string() = default;
  ~basic_string() noexcept { reset(); }

  basic_string(const_pointer str) : basic_string(str, strlen(str)) {}
  basic_string(const basic_string &rhs) : basic_string(rhs.m_ptr, rhs.size(), rhs.capacity()) {}

  basic_string(basic_string &&rhs) noexcept { swap(rhs); }

  basic_string &operator=(const basic_string &rhs) {
    if (this != &rhs) {
      auto tmp = rhs;
      swap(tmp);
    }
    return *this;
  }

  basic_string &operator=(basic_string &&rhs) noexcept {
    if (this != &rhs) {
      auto tmp = move(rhs);
      swap(tmp);
    }
    return *this;
  }

  void swap(basic_string &rhs) noexcept {
    using aria::swap;
    swap(m_alloc, rhs.m_alloc);
    swap(m_ptr, rhs.m_ptr);
    swap(m_size, rhs.m_size);
    swap(m_capacity, rhs.m_capacity);
  }

  size_type size() const noexcept { return m_size; }
  size_type capacity() const noexcept { return m_capacity; }
  bool empty() const noexcept { return m_size == 0; }
  reference back() noexcept { return *get(m_size - 1); }
  const_reference back() const noexcept { return *get(m_size - 1); }
  reference operator[](size_type i) { return *get(i); }
  const_reference &operator[](size_type i) const { return *get(i); }
  void clear() noexcept { m_size = 0; }
  void pop_back() { m_size--; }
  const CharT *data() const noexcept { return m_ptr; }
  CharT *data() noexcept { return m_ptr; }
  const CharT *c_str() const noexcept { return m_ptr; }

  auto begin() const noexcept { return const_iterator(get(0)); }
  auto end() const noexcept { return const_iterator(get(m_size)); }
  auto begin() noexcept { return iterator(get(0)); }
  auto end() noexcept { return iterator(get(m_size)); }
  auto rbegin() const noexcept { return const_reverse_iterator(end()); }
  auto rend() const noexcept { return const_reverse_iterator(begin()); }
  auto rbegin() noexcept { return reverse_iterator(end()); }
  auto rend() noexcept { return reverse_iterator(begin()); }

  void reserve(size_type new_cap) {
    if (new_cap <= capacity())
      return;
    auto new_ptr = m_alloc.allocate(new_cap);
    memcpy(new_ptr, m_ptr, m_size * sizeof(value_type));
    m_alloc.deallocate(m_ptr, m_capacity);
    m_capacity = new_cap;
    m_ptr = new_ptr;
  }

  bool operator==(const basic_string &rhs) const noexcept {
    if (this == &rhs)
      return true;
    return equal(begin(), end(), rhs.begin(), rhs.end());
  }

  basic_string substr(size_type pos = 0, size_type count = npos) const {
    if (pos >= size())
      throw std::out_of_range("basic_string::substr pos >= size()");

    count = std::min(count, size() - pos);
    return basic_string(m_ptr + pos, count, count);
  }

  basic_string &operator+=(CharT ch) {
    reserve_more(1);
    *get(m_size) = ch;
    m_size += 1;
    return *this;
  }

  basic_string &operator+=(const basic_string &rhs) { return append(rhs.m_ptr, rhs.size()); }
  basic_string &operator+=(const_pointer rhs) { return append(rhs, strlen(rhs)); }

private:
  pointer get(size_type i) { return m_ptr + i; }
  const pointer get(size_type i) const { return m_ptr + i; }

  basic_string(const_pointer str, size_type size, size_type cap = 0) {
    if (cap == 0)
      cap = size;
    reserve(cap);
    copy(str, size);
  }

  basic_string &append(const_pointer rhs, size_t added_size) {
    reserve_more(added_size);
    memcpy(m_ptr + m_size, rhs, added_size * sizeof(value_type));
    m_size += added_size;
    return *this;
  }

  void reset() noexcept {
    if (m_ptr) {
      m_alloc.deallocate(m_ptr, m_capacity);
      m_ptr = nullptr;
      m_size = 0;
      m_capacity = 0;
    }
  }

  void copy(const_pointer src, size_type size) {
    if (src) {
      memcpy(m_ptr, src, size * sizeof(value_type));
      m_size = size;
    }
  }

  void reserve_more(size_type added_size) {
    size_type new_capacity = std::max(size_type(1), capacity());
    const size_type new_size = size() + added_size;
    while (new_size > new_capacity)
      new_capacity *= 2;
    reserve(new_capacity);
  }

  void add_null_teminate() noexcept { *get(m_size) = value_type(0); }

  Allocator m_alloc;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;
};

using string = basic_string<char>;

} // namespace aria
