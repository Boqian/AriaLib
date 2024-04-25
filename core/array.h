#pragma once
#include "algorithm.h"
#include "exception.h"
#include "iterator.h"
#include "utility.h"

namespace aria {

template <class T, size_t N> class array {
public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  constexpr reference operator[](size_t i) { return arr[i]; }
  constexpr const_reference operator[](size_t i) const { return arr[i]; }

  constexpr const_reference at(size_t i) const {
    if (i >= N)
      throw std::out_of_range("");
    return arr[i];
  }
  constexpr reference at(size_t i) { return const_cast<reference>(as_const(*this).at(i)); }

  constexpr bool empty() const noexcept { return N == 0; }
  constexpr size_type size() const noexcept { return N; }
  constexpr size_type max_size() const noexcept { return N; }
  constexpr reference front() { return arr[0]; }
  constexpr const_reference front() const { return arr[0]; }
  constexpr reference back() { return arr[N - 1]; }
  constexpr const_reference back() const { return arr[N - 1]; }

  constexpr iterator begin() { return arr; }
  constexpr const_iterator begin() const { return arr; }
  constexpr iterator end() { return arr + N; }
  constexpr const_iterator end() const { return arr + N; }
  constexpr auto rbegin() const noexcept { return const_reverse_iterator(end()); }
  constexpr auto rend() const noexcept { return const_reverse_iterator(begin()); }
  constexpr auto rbegin() noexcept { return reverse_iterator(end()); }
  constexpr auto rend() noexcept { return reverse_iterator(begin()); }

  void swap(array &rhs) noexcept {
    using aria::swap;
    for (int i = 0; i < N; i++)
      swap(arr[i], rhs[i]);
  }

  T arr[N];
};

template <class T, size_t N> [[nodiscard]] constexpr bool operator==(const array<T, N> &a, const array<T, N> &b) {
  return equal(a.begin(), a.end(), b.begin());
}

template <class T, size_t N> [[nodiscard]] constexpr auto operator<=>(const array<T, N> &a, const array<T, N> &b) {
  return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
}

} // namespace aria