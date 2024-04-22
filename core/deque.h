#pragma once
#include "algorithm.h" //aria::max
#include "allocator.h"
#include "iterator.h"
#include "utility.h"
#include "vector.h"

namespace aria {

template <class T, class Allocator = allocator<T>> class deque {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using allocator_type = Allocator;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;

  deque() noexcept = default;

  deque(initializer_list<T> init) {
    for (const auto &x : init)
      push_back(x);
  }

  ~deque() {
    for (int i = 0; i < size(); i++)
      destroy_at(get(i));
    for (auto p : m_buckets) {
      m_alloc.deallocate(p, s_bucket_size);
    }
  }

  deque(const deque &rhs)
      : m_buckets(rhs.m_buckets.size()), m_start(rhs.m_start), m_end(rhs.m_end), m_bucket_start_index(rhs.m_bucket_start_index),
        m_alloc(rhs.m_alloc) {
    for (int i = m_bucket_start_index; i < m_buckets.size(); i++) {
      m_buckets[i] = create_bucket();
    }
    for (int i = 0; i < rhs.size(); i++)
      construct_at(get(i), rhs[i]);
  }

  deque &operator=(const deque &rhs) {
    if (&rhs != this) {
      auto temp = rhs;
      swap(temp);
    }
    return *this;
  }

  deque(deque &&rhs) noexcept { swap(rhs); }

  deque &operator=(deque &&rhs) noexcept {
    if (&rhs != this) {
      auto temp = move(rhs);
      swap(temp);
    }
    return *this;
  }

  void push_back(const_reference val) {
    if (m_end == s_bucket_size) {
      m_buckets.push_back(create_bucket());
      m_end = 0;
    }
    construct_at(&(m_buckets.back()[m_end]), val);
    m_end++;
  }

  void pop_back() {
    destroy_at(m_buckets.back() + m_end - 1);
    if (--m_end == 0) {
      deallocate_bucket(m_buckets.size() - 1);
      m_buckets.pop_back();
      m_end = s_bucket_size;
    }
  }

  void push_front(const_reference val) {
    if (m_start == 0) {
      if (m_bucket_start_index == 0) {
        expand_buckets(1);
      }
      m_buckets[--m_bucket_start_index] = create_bucket();
      m_start = s_bucket_size;
    }
    --m_start;
    construct_at(&(m_buckets[m_bucket_start_index][m_start]), val);
  }

  void pop_front() {
    destroy_at(m_buckets[m_bucket_start_index] + m_start);
    if (++m_start == s_bucket_size) {
      deallocate_bucket(m_bucket_start_index);
      ++m_bucket_start_index;
      m_start = 0;
    }
  }

  bool empty() const noexcept { return size() == 0; }
  size_type size() const noexcept {
    if (m_buckets.empty())
      return 0;
    return (m_buckets.size() - m_bucket_start_index - 1) * s_bucket_size + m_end - m_start;
  }

  const_reference operator[](size_type i) const { return *get(i); }
  reference operator[](size_type i) { return const_cast<reference>(*get(i)); }

  reference back() noexcept { return m_buckets.back()[m_end - 1]; }
  const_reference back() const noexcept { return m_buckets.back()[m_end - 1]; }
  reference front() noexcept { return m_buckets[m_bucket_start_index][m_start]; }
  const_reference front() const noexcept { m_buckets[m_bucket_start_index][m_start]; }

  void swap(deque &rhs) noexcept {
    aria::swap(m_buckets, rhs.m_buckets);
    aria::swap(m_start, rhs.m_start);
    aria::swap(m_end, rhs.m_end);
    aria::swap(m_bucket_start_index, rhs.m_bucket_start_index);
    aria::swap(m_alloc, rhs.m_alloc);
  }

  bool operator==(const deque &rhs) const noexcept {
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

private:
  inline static constexpr size_type s_bucket_size = max<size_type>(64 / sizeof(T), 16);
  inline static constexpr size_type s_init_num_buckets = 2;
  inline static constexpr size_type s_init_bucket_start_index = s_init_num_buckets - 1;

  pointer create_bucket() { return m_alloc.allocate(s_bucket_size); }

  void deallocate_bucket(size_type i) {
    m_alloc.deallocate(m_buckets[i], s_bucket_size);
    m_buckets[i] = nullptr;
  }

  pointer get(size_type i) const {
    if (m_start + i < s_bucket_size)
      return m_buckets[m_bucket_start_index] + m_start + i;
    i -= s_bucket_size - m_start;
    auto bucket_index = 1 + (i / s_bucket_size), index = i % s_bucket_size;
    return m_buckets[bucket_index + m_bucket_start_index] + index;
  }

  pointer get_bucket(size_type i) const { return m_buckets[i + m_bucket_start_index]; }
  size_type buckets_size() const { return m_buckets.size() - m_bucket_start_index; }

  void expand_buckets(size_type n) {
    auto new_size = buckets_size() + n;
    vector<pointer> new_buckets(new_size, nullptr);
    new_buckets.reserve(new_size * 3);
    for (int i = 0; i < m_buckets.size(); i++)
      new_buckets.push_back(get_bucket(i));
    m_buckets = new_buckets;
    m_bucket_start_index = new_size;
  }

  vector<pointer> m_buckets;
  size_type m_start{};
  size_type m_end{s_bucket_size};
  size_type m_bucket_start_index{};
  Allocator m_alloc;
};

} // namespace aria