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

  bool empty() const noexcept { return size() == 0; }
  size_type size() const noexcept {
    if (m_buckets.empty())
      return 0;
    return (m_buckets.size() - m_bucket_start_index - 1) * bucket_size + m_end - m_start;
  }

  const_reference operator[](size_type i) const {
    if (i < bucket_size - m_start)
      return m_buckets[m_bucket_start_index][i + m_start];
    i -= bucket_size - m_start;
    auto bucket_index = 1 + (i / bucket_size), index = i % bucket_size;
    return m_buckets[bucket_index + m_bucket_start_index][index];
  }

  reference operator[](size_type i) { return const_cast<reference>(add_const(*this).operator[](i)); }

private:
  inline static constexpr size_type bucket_size = max<size_type>(sizeof(T), 256) * 16;

  vector<pointer> m_buckets;
  size_type m_start{};
  size_type m_end{};
  size_type m_bucket_start_index{};
};

} // namespace aria