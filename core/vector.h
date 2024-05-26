#pragma once

#include "algorithm.h"
#include "allocator.h"
#include "exception.h"
#include "iterator.h"
#include "utility.h"

namespace aria {

template <class T, class Allocator = allocator<T>> class vector : public iterable {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using allocator_type = Allocator;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using iterator = array_iterator<T>;
  using const_iterator = basic_const_iterator<iterator>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  constexpr vector() noexcept = default;

  constexpr ~vector() {
    clear();
    m_alloc.deallocate(m_ptr, m_capacity);
  }

  constexpr vector(initializer_list<T> init) {
    reserve(init.size());
    m_size = init.size();
    for (size_type i = 0; auto &value : init) {
      construct_at(get(i++), value);
    }
  }

  constexpr vector(size_type n, const T &value = T()) {
    reserve(n);
    m_size += n;
    for (int i = 0; i < n; i++) {
      construct_at(get(i), value);
    }
  }

  constexpr vector(const vector &rhs) {
    reserve(rhs.size());
    m_size = rhs.size();
    for (int i = 0; i < m_size; i++) {
      construct_at(get(i), rhs[i]);
    }
  }

  constexpr vector(vector &&rhs) noexcept { swap(rhs); }

  constexpr vector &operator=(const vector &rhs) {
    if (this != &rhs) {
      auto v = rhs;
      swap(v);
    }
    return *this;
  }

  constexpr vector &operator=(vector &&rhs) noexcept {
    if (this != &rhs) {
      auto v = move(rhs);
      swap(v);
    }
    return *this;
  }

  constexpr void push_back(const T &value) {
    reserve(new_capacity(1));
    construct_at(get(m_size), value);
    m_size++;
  }

  template <class... Args> constexpr void emplace_back(Args &&...args) {
    reserve(new_capacity(1));
    aria::construct_at(get(m_size), ::aria::forward<Args>(args)...);
    m_size++;
  }

  constexpr void pop_back() {
    if (m_size > 0) {
      m_size--;
      decstuct_at(m_size);
    }
  }

  constexpr void reserve(size_type new_cap) {
    if (new_cap > capacity())
      reallocate(new_cap);
  }

  constexpr void clear() {
    for (int i = 0; i < m_size; i++)
      decstuct_at(i);
    m_size = 0;
  }

  constexpr void shrink_to_fit() {
    if (capacity() > size())
      reallocate(size());
  }

  constexpr size_type size() const noexcept { return m_size; }
  constexpr size_type capacity() const noexcept { return m_capacity; }
  constexpr bool empty() const noexcept { return size() == 0; }
  constexpr reference back() noexcept { return *get(m_size - 1); }
  constexpr const_reference back() const noexcept { return *get(m_size - 1); }

  constexpr reference operator[](size_type i) { return *get(i); }
  constexpr const_reference operator[](size_type i) const { return *get(i); }
  constexpr reference at(size_type i) {
    check_position(i);
    return *get(i);
  }
  constexpr const_reference at(size_type i) const {
    check_position(i);
    return *get(i);
  }

  constexpr pointer data() noexcept { return m_ptr; }
  constexpr const_pointer data() const noexcept { return m_ptr; }

  constexpr void swap(vector &rhs) noexcept {
    using aria::swap;
    swap(m_size, rhs.m_size);
    swap(m_capacity, rhs.m_capacity);
    swap(m_ptr, rhs.m_ptr);
    swap(m_alloc, rhs.m_alloc);
  }

  constexpr auto begin() const noexcept { return const_iterator(get(0)); }
  constexpr auto end() const noexcept { return const_iterator(get(m_size)); }
  constexpr auto begin() noexcept { return iterator(get(0)); }
  constexpr auto end() noexcept { return iterator(get(m_size)); }

  constexpr iterator erase(const_iterator first, const_iterator last)
    requires is_move_assignable_v<T>
  {
    if (first == end())
      return end();
    auto d = distance(first, last);
    if (d == 0)
      return last;
    for (iterator it = last; it != end(); ++it) {
      *(it - d) = move(*it);
    }
    while (d--)
      pop_back();
    return first;
  }

  constexpr iterator erase(const_iterator pos)
    requires is_move_assignable_v<T>
  {
    if (pos == end())
      return end();
    return erase(pos, pos + 1);
  }

private:
  Allocator m_alloc;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;

  constexpr pointer get(size_type i) { return m_ptr + i; }
  constexpr const_pointer get(size_type i) const { return m_ptr + i; }

  constexpr void check_position(size_type i) const {
    if (i >= m_size)
      throw std::out_of_range("");
  }

  constexpr void decstuct_at(size_type i) { destroy_at(get(i)); }

  constexpr void reallocate(size_type cap) {
    auto p = m_alloc.allocate(cap);
    for (int i = 0; i < m_size; i++) {
      if constexpr (is_move_contructible_v<T>) {
        construct_at(p + i, move(*(m_ptr + i)));
      } else {
        construct_at(p + i, *(m_ptr + i));
      }
    }
    for (int i = 0; i < m_size; i++) {
      decstuct_at(i);
    }
    m_alloc.deallocate(m_ptr, m_capacity);
    m_ptr = p;
    m_capacity = cap;
  }

  constexpr size_type new_capacity(size_type add_size) {
    if (capacity() == 0)
      return add_size;
    auto min_capacity = m_size + add_size;
    auto new_capacity = capacity();
    while (new_capacity < min_capacity)
      new_capacity *= 2;
    return new_capacity;
  }
};

template <class T, class Alloc> [[nodiscard]] constexpr bool operator==(const vector<T, Alloc> &a, const vector<T, Alloc> &b) {
  return equal(a.begin(), a.end(), b.begin(), b.end());
}

template <class T, class Alloc> [[nodiscard]] constexpr auto operator<=>(const vector<T, Alloc> &a, const vector<T, Alloc> &b) {
  return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
}

template <class T, class A> constexpr void swap(vector<T, A> &a, vector<T, A> &b) { a.swap(b); }

template <class T, class Alloc, class U = T> constexpr vector<T, Alloc>::size_type erase(vector<T, Alloc> &v, const U &value) {
  typename vector<T, Alloc>::size_type i = 0, index = 0;
  for (; i < v.size(); ++i) {
    if (v[i] != value) {
      v[index++] = move(v[i]);
    }
  }
  auto res = v.size() - index;
  v.erase(begin(v) + index, end(v));
  return res;
}

template <class T, class Alloc, class Pred> constexpr vector<T, Alloc>::size_type erase_if(vector<T, Alloc> &v, Pred pred) {
  typename vector<T, Alloc>::size_type i = 0, index = 0;
  for (; i < v.size(); ++i) {
    if (!pred(v[i])) {
      v[index++] = move(v[i]);
    }
  }
  auto res = v.size() - index;
  v.erase(begin(v) + index, end(v));
  return res;
}

} // namespace aria