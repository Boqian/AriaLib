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

template <class T, size_t Extent = dynamic_extent> class span : public iterable, private _span::extent_type<T, Extent> {
public:
  using element_type = T;
  using value_type = remove_cv_t<T>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = array_iterator<T>;
  using const_iterator = basic_const_iterator<iterator>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  static constexpr size_t extent = Extent;

  constexpr span() noexcept = default;

  template <contiguous_iterator It> constexpr span(It first, size_type count) : base(to_address(first), count) {}
  template <contiguous_iterator It, class End> requires ranges::sized_sentinel_for<End, It>
  constexpr span(It first, End last) : base(to_address(first), last - first) {}
  template <ranges::range R> constexpr span(R &&r) : base(ranges::data(r), ranges::size(r)) {}

  template <class U, size_t N> requires convertible_to<U, T> && (N == Extent)
  constexpr span(array<U, N> &arr) noexcept : base(arr.data(), N) {}

  constexpr bool empty() const noexcept { return m_size == 0; }
  constexpr size_type size() const noexcept { return m_size; }
  constexpr reference front() const { return m_data[0]; }
  constexpr reference back() const { return m_data[size() - 1]; }
  constexpr pointer data() const noexcept { return m_data; }
  constexpr reference operator[](size_type idx) const { return *(m_data + idx); }
  constexpr iterator begin() const noexcept { return iterator(m_data); }
  constexpr iterator end() const noexcept { return iterator(m_data + m_size); }

  template <size_t Count> constexpr span<element_type, Count> first() const { return span<element_type, Count>(begin(), Count); }
  constexpr span<element_type, dynamic_extent> first(size_t count) const { return span(begin(), count); }
  template <size_t Count> constexpr span<element_type, Count> last() const { return span<element_type, Count>(end() - Count, Count); }
  constexpr span<element_type, dynamic_extent> last(size_t count) const { return span(end() - count, count); }

  template <size_t Offset, size_t Count = dynamic_extent> constexpr auto subspan() const {
    if constexpr (Count == dynamic_extent) {
      return span<element_type, dynamic_extent>(begin() + Offset, m_size - Offset);
    } else {
      return span<element_type, Count>(begin() + Offset, Count);
    }
  }

  constexpr span<element_type, dynamic_extent> subspan(size_type Offset, size_type Count = dynamic_extent) const {
    return span<element_type, dynamic_extent>(begin() + Offset, Count == dynamic_extent ? m_size - Offset : Count);
  }

private:
  using base = _span::extent_type<T, Extent>;
  using base::m_data;
  using base::m_size;
};

template <class It, class EndOrSize> span(It, EndOrSize) -> span<remove_reference_t<iter_reference_t<It>>>;
template <class T, size_t Size> span(T (&)[Size]) -> span<T, Size>;
template <class T, size_t Size> span(array<T, Size> &) -> span<T, Size>;
template <class T, size_t Size> span(const array<T, Size> &) -> span<const T, Size>;
template <ranges::range R> span(R &&) -> span<remove_reference_t<ranges::range_reference_t<R>>>;

} // namespace aria