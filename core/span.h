#pragma once

#include "array.h"
#include "iterator.h"
#include "ranges.h"

namespace aria {

inline constexpr size_t dynamic_extent = -1;

namespace _span {
template <class T, size_t Extent> struct extent_type {
  extent_type() = default;
  extent_type(T *p, size_t) : m_data(p) {}
  T *m_data = nullptr;
  static constexpr size_t m_size = Extent;
};

template <class T> struct extent_type<T, dynamic_extent> {
  extent_type() = default;
  extent_type(T *p, size_t size) : m_data(p), m_size(size) {}
  T *m_data = nullptr;
  size_t m_size = 0;
};

} // namespace _span

template <class T, size_t Extent = dynamic_extent>
class span : private _span::extent_type<T, Extent>

{
public:
  using element_type = T;
  using value_type = remove_cv_t<T>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  static constexpr size_t extent = Extent;

  constexpr span() noexcept = default;

  // todo contiguous_iterator
  template <class It> constexpr span(It first, size_type count) : base(to_address(first), count) {}
  template <class It, class End> constexpr span(It first, End last) : base(to_address(first), last - first) {}

  // todo ranges::size
  // template <ranges::range R> constexpr span(R &&range) : base(ranges::begin(range), ranges::size(range)) {}

  template <class U, size_t N>
    requires convertible_to<U, T> && (N == Extent)
  constexpr span(array<U, N> &arr) noexcept : base(arr.data(), N) {}

  constexpr bool empty() const noexcept { return m_size == 0; }
  constexpr size_type size() const noexcept { return m_size; }
  constexpr reference front() const { return m_data[0]; }
  constexpr reference back() const { return m_data[size() - 1]; }
  constexpr pointer data() const noexcept { return m_data; }
  constexpr reference operator[](size_type idx) const { return *(m_data + idx); }

  template <size_t Count> constexpr span<element_type, Count> first() const {}

private:
  using base = _span::extent_type<T, Extent>;
  using base::m_data;
  using base::m_size;
};

template <class T, size_t Size> span(T (&)[Size]) -> span<T, Size>;
template <class T, size_t Size> span(array<T, Size> &) -> span<T, Size>;
template <class T, size_t Size> span(const array<T, Size> &) -> span<const T, Size>;

} // namespace aria