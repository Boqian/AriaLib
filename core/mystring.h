#pragma once

#include "algorithm.h"
#include "allocator.h"
#include "bit.h"
#include "iterator.h"
#include "stdexcept.h"
#include "type_list.h"
#include "utility.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace aria {

template <class CharT, class Allocator = allocator<CharT>> class basic_string : public iterable_mixin {
public:
  using size_type = size_t;
  using value_type = CharT;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = array_iterator<CharT>;
  using const_iterator = basic_const_iterator<iterator>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;
  static constexpr size_type npos = -1;

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

    const bool lhs_on_stack = on_stack(), rhs_on_stack = rhs.on_stack();
    swap(m_small_str, rhs.m_small_str);
    swap(m_alloc, rhs.m_alloc);
    swap(m_ptr, rhs.m_ptr);
    swap(m_size, rhs.m_size);
    swap(m_capacity, rhs.m_capacity);
    if (lhs_on_stack) {
      rhs.m_ptr = rhs.m_small_str;
    }
    if (rhs_on_stack) {
      m_ptr = m_small_str;
    }
  }

  size_type size() const noexcept { return m_size; }
  size_type length() const noexcept { return size(); }
  size_type capacity() const noexcept { return (m_capacity == 0) ? 0 : m_capacity - 1; }
  bool empty() const noexcept { return size() == 0; }
  reference back() noexcept { return *get(size() - 1); }
  const_reference back() const noexcept { return *get(size() - 1); }
  reference operator[](size_type i) { return *get(i); }
  const_reference &operator[](size_type i) const { return *get(i); }
  void clear() noexcept { set_size(0); }
  void pop_back() { add_size(-1); }
  void push_back(CharT ch) { operator+=(ch); }
  const CharT *data() const noexcept { return m_ptr; }
  CharT *data() noexcept { return m_ptr; }
  const CharT *c_str() const noexcept { return m_ptr; }

  auto begin() const noexcept { return const_iterator(get(0)); }
  auto end() const noexcept { return const_iterator(get(size())); }
  auto begin() noexcept { return iterator(get(0)); }
  auto end() noexcept { return iterator(get(size())); }

  void reserve(size_type new_cap) {
    if (new_cap <= m_capacity)
      return;
    auto new_ptr = m_alloc.allocate(new_cap);
    memcpy(new_ptr, m_ptr, (size() + 1) * sizeof(value_type));
    if (!on_stack())
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
      throw out_of_range("basic_string::substr pos >= size()");

    count = min(count, size() - pos);
    return basic_string(m_ptr + pos, count, count);
  }

  basic_string &operator+=(CharT ch) {
    reserve_more(1);
    *get(size()) = ch;
    add_size(1);
    return *this;
  }

  basic_string &operator+=(const basic_string &rhs) { return append(rhs.m_ptr, rhs.size()); }
  basic_string &operator+=(const_pointer rhs) { return append(rhs, strlen(rhs)); }

  size_type find(const basic_string &str, size_type pos = 0) const {
    if (pos + str.size() > size())
      return npos;
    const auto ptr = strstr(data() + pos, str.data());
    return ptr ? (ptr - m_ptr) : npos;
  }

  size_type find(const CharT *s, size_type pos = 0) const {
    if (pos + strlen(s) > size())
      return npos;
    const auto ptr = strstr(data() + pos, s);
    return ptr ? (ptr - m_ptr) : npos;
  }

private:
  pointer get(size_type i) { return m_ptr + i; }
  const pointer get(size_type i) const { return m_ptr + i; }

  basic_string(const_pointer str, size_type size, size_type cap = 0) {
    cap = max(cap, size + 1);
    if (cap > s_stack_cap)
      reserve(cap);
    copy(str, size);
  }

  basic_string &append(const_pointer rhs, size_t added_size) {
    reserve_more(added_size);
    memcpy(m_ptr + size(), rhs, added_size * sizeof(value_type));
    add_size(added_size);
    return *this;
  }

  void reset() noexcept {
    if (!on_stack()) {
      m_alloc.deallocate(m_ptr, m_capacity);
    }
    m_ptr = m_small_str;
    set_size(0);
    m_capacity = s_stack_cap;
  }

  void copy(const_pointer src, size_type size) {
    if (src) {
      memcpy(m_ptr, src, size * sizeof(value_type));
      set_size(size);
    }
  }

  void reserve_more(size_type added_size) {
    if (auto new_size = size() + added_size; new_size > capacity())
      reserve(bit_ceil(new_size));
  }

  void set_size(size_t new_size) {
    m_size = new_size;
    add_null_teminate();
  }

  void add_size(size_t size) {
    m_size += size;
    add_null_teminate();
  }

  void add_null_teminate() noexcept { *get(size()) = value_type(0); }

  bool on_stack() const noexcept { return m_ptr == m_small_str; }

  static constexpr size_t s_stack_cap = 16;
  value_type m_small_str[s_stack_cap]{};
  pointer m_ptr = m_small_str;
  size_type m_size = 0;
  size_type m_capacity = s_stack_cap;
  Allocator m_alloc;
};

using string = basic_string<char>;

template <class CharT, class Alloc> void swap(basic_string<CharT, Alloc> &a, basic_string<CharT, Alloc> &b) noexcept { a.swap(b); }

template <class CharT, class Alloc>
[[nodiscard]] constexpr auto operator<=>(const basic_string<CharT, Alloc> &a, const basic_string<CharT, Alloc> &b) {
  return lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
}

template <class T, class CFunc> T _to_int(CFunc cfunc, const string &str, size_t *pos = nullptr, int base = 10) {
  int &errno_ref = errno; // Nonzero cost, pay it once
  const char *ptr = str.c_str();
  char *end_ptr;
  errno_ref = 0;
  const auto ans = cfunc(ptr, &end_ptr, base);
  if (ptr == end_ptr) {
    throw invalid_argument("invalid stoi argument");
  }
  if (errno_ref == ERANGE) {
    throw out_of_range("stoi argument out of range");
  }
  if (pos) {
    *pos = static_cast<size_t>(end_ptr - ptr);
  }
  return static_cast<T>(ans);
}

template <class T, class CFunc> T _to_float(CFunc cfunc, const string &str, size_t *pos = nullptr) {
  int &errno_ref = errno; // Nonzero cost, pay it once
  const char *ptr = str.c_str();
  char *end_ptr;
  errno_ref = 0;
  const auto ans = cfunc(ptr, &end_ptr);
  if (ptr == end_ptr) {
    throw invalid_argument("invalid stoi argument");
  }
  if (errno_ref == ERANGE) {
    throw out_of_range("stoi argument out of range");
  }
  if (pos) {
    *pos = static_cast<size_t>(end_ptr - ptr);
  }
  return static_cast<T>(ans);
}

int stoi(const string &str, size_t *pos = nullptr, int base = 10) { return _to_int<int>(strtol, str, pos, base); }
long stol(const string &str, size_t *pos = nullptr, int base = 10) { return _to_int<long>(strtol, str, pos, base); }
long long stoll(const string &str, size_t *pos = nullptr, int base = 10) { return _to_int<long long>(strtoll, str, pos, base); }
unsigned long stoul(const string &str, size_t *pos = nullptr, int base = 10) { return _to_int<unsigned long>(strtoul, str, pos, base); }
auto stoull(const string &str, size_t *pos = nullptr, int base = 10) { return _to_int<unsigned long long>(strtoull, str, pos, base); }
float stof(const string &str, size_t *pos = nullptr) { return _to_float<float>(strtof, str, pos); }
double stod(const string &str, size_t *pos = nullptr) { return _to_float<double>(strtod, str, pos); }
long double stold(const string &str, size_t *pos = nullptr) { return _to_float<long double>(strtold, str, pos); }

template <arithmetic T> string to_string(T x) {
  using num_type_list = type_list<int, long, long long, unsigned, unsigned long, unsigned long, float, double, long double>;
  static constexpr const char *const fmt_array[] = {"%d", "%ld", "%lld", "%u", "%lu", "%llu", "%f", "%f", "%Lf"};
  if constexpr (!exact_one_match<T, num_type_list>()) {
    return to_string<int>(x);
  } else {
    static constexpr const char *fmt = fmt_array[index_of<T>(num_type_list())];
    char buffer[100];
    sprintf(buffer, fmt, x);
    return string(buffer);
  }
}

} // namespace aria
