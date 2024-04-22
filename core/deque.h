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

  ~deque() {
    if (buckets_size() == 0)
      return;

    if (buckets_size() == 1) {
      destroy_bucket(get_bucket(0), m_start, m_end);
    } else {
      destroy_bucket(get_bucket(0), m_start);
      destroy_bucket(m_buckets.back(), 0, m_end);
      for (int i = 1; i < buckets_size() - 1; i++) {
        destroy_bucket(get_bucket(i));
      }
    }
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
    destroy_at(m_buckets.back()[m_end - 1]);
    if (--m_end == 0) {
      m_buckets.pop_back();
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
    destroy_at(m_buckets[m_bucket_start_index][m_start]);
    if (++m_start == s_bucket_size) {
      m_buckets[++m_bucket_start_index] = nullptr;
    }
  }

  bool empty() const noexcept { return size() == 0; }
  size_type size() const noexcept {
    if (m_buckets.empty())
      return 0;
    return (m_buckets.size() - m_bucket_start_index - 1) * s_bucket_size + m_end - m_start;
  }

  const_reference operator[](size_type i) const {
    if (i < s_bucket_size - m_start)
      return m_buckets[m_bucket_start_index][i + m_start];
    i -= s_bucket_size - m_start;
    auto bucket_index = 1 + (i / s_bucket_size), index = i % s_bucket_size;
    return m_buckets[bucket_index + m_bucket_start_index][index];
  }

  reference operator[](size_type i) { return const_cast<reference>(add_const(*this).operator[](i)); }

private:
  inline static constexpr size_type s_bucket_size = max<size_type>(256 / sizeof(T), 16);
  inline static constexpr size_type s_init_num_buckets = 2;
  inline static constexpr size_type s_init_bucket_start_index = s_init_num_buckets - 1;

  pointer create_bucket() { return m_alloc.allocate(s_bucket_size); }

  void destroy_bucket(pointer p, size_type start = 0, size_type end = s_bucket_size) {
    for (auto i = start; i < end; i++)
      destroy_at(p + i);
    m_alloc.deallocate(p, s_bucket_size);
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