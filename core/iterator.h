#pragma once

#include "concepts.h"
#include "cstddef.h"
#include <compare>

namespace aria {

template <class I>
concept weakly_incrementable = std::movable<I> && requires(I i) {
  { ++i } -> same_as<I &>;
  { i++ };
};

template <class I>
concept input_or_output_iterator = weakly_incrementable<I> && requires(I i) {
  { *i } -> not_void;
};

// todo indirect_readable, incrementable
template <class I>
concept forward_iterator = requires(I i) {
  { ++i } -> same_as<I &>;
  { i++ };
};

template <class I>
concept bidirectional_iterator = forward_iterator<I> && requires(I i) {
  { i-- } -> same_as<I>;
  { --i } -> same_as<I &>;
};

template <class I>
concept random_access_iterator = bidirectional_iterator<I> && requires(I i, I j, ptrdiff_t n) {
  { i += n } -> same_as<I &>;
  { i -= n } -> same_as<I &>;
  { i + n } -> same_as<I>;
  { i - n } -> same_as<I>;
  { i - j } -> same_as<ptrdiff_t>;
};

template <class InputIt> constexpr ptrdiff_t distance(InputIt first, InputIt last) {
  if constexpr (random_access_iterator<InputIt>) {
    return last - first;
  } else {
    ptrdiff_t d = 0;
    for (; first != last; ++first, ++d)
      ;
    return d;
  }
}

template <class InputIt> constexpr InputIt advance(InputIt i, ptrdiff_t n) {
  if constexpr (random_access_iterator<InputIt>) {
    return i + n;
  } else {
    if (n > 0) {
      while (n--)
        ++i;
    } else {
      while (n++)
        --i;
    }
    return i;
  }
}

template <class InputIt> constexpr InputIt next(InputIt it, ptrdiff_t n = 1) { return advance(it, n); }
template <class InputIt> constexpr InputIt prev(InputIt it, ptrdiff_t n = 1) { return advance(it, -n); }

template <class Container> constexpr auto begin(Container &cont) { return cont.begin(); }
template <class Container> constexpr auto cbegin(const Container &cont) { return cont.begin(); }
template <class Container> constexpr auto end(Container &cont) { return cont.end(); }
template <class Container> constexpr auto cend(const Container &cont) { return cont.end(); }
template <class Container> constexpr auto rbegin(Container &cont) { return cont.rbegin(); }
template <class Container> constexpr auto crbegin(const Container &cont) { return cont.rbegin(); }
template <class Container> constexpr auto rend(Container &cont) { return cont.rend(); }
template <class Container> constexpr auto crend(const Container &cont) { return cont.rend(); }

template <class T, size_t N> constexpr T *begin(T (&array)[N]) noexcept { return array; }
template <class T, size_t N> constexpr T *end(T (&array)[N]) noexcept { return array + N; }

template <class iter> class basic_const_iterator {
public:
  using value_type = typename iter::value_type;
  using pointer = const value_type *;
  using reference = const value_type &;
  using difference_type = typename iter::difference_type;

  basic_const_iterator() = default;
  basic_const_iterator(iter a) : it(a) {}

  template <class... Args>
    requires constructible_from<iter, Args...>
  explicit basic_const_iterator(Args &&...args) : it(forward<Args>(args)...) {}

  operator iter() const noexcept { return it; }

  reference operator*() const noexcept { return it.operator*(); }
  pointer operator->() const noexcept { return it.operator->(); }

  basic_const_iterator &operator++() noexcept {
    it.operator++();
    return *this;
  }
  basic_const_iterator operator++(int) noexcept {
    auto temp = *this;
    it.operator++();
    return temp;
  }
  basic_const_iterator &operator--() noexcept {
    it.operator--();
    return *this;
  }
  basic_const_iterator operator--(int) noexcept {
    auto temp = *this;
    it.operator--();
    return temp;
  }

  basic_const_iterator &operator+=(const difference_type d)
    requires random_access_iterator<iter>
  {
    it += d;
    return *this;
  }

  basic_const_iterator &operator-=(const difference_type d)
    requires random_access_iterator<iter>
  {
    it.operator-=(d);
    return *this;
  }

  basic_const_iterator operator+(const difference_type d) const
    requires random_access_iterator<iter>
  {
    auto temp = *this;
    temp += d;
    return temp;
  }

  difference_type operator-(const basic_const_iterator &rhs) const noexcept
    requires random_access_iterator<iter>
  {
    return it - rhs.it;
  }

  basic_const_iterator operator-(const difference_type d) const
    requires random_access_iterator<iter>
  {
    auto temp = *this;
    temp -= d;
    return temp;
  }

  auto operator<=>(const basic_const_iterator &) const noexcept = default;

private:
  iter it;
};

template <class iter> constexpr bool operator==(iter a, basic_const_iterator<iter> b) noexcept { return static_cast<iter>(b) == a; }

// use const iterator type to implement mutable iterator type
template <class const_iterator> class mutable_iterator {
public:
  using value_type = typename const_iterator::value_type;
  using pointer = value_type *;
  using reference = value_type &;
  using difference_type = typename const_iterator::difference_type;

  mutable_iterator() = default;
  mutable_iterator(const_iterator a) : it(a) {}

  template <class... Args>
    requires constructible_from<const_iterator, Args...>
  explicit mutable_iterator(Args &&...args) : it(forward<Args>(args)...) {}

  operator const_iterator() const noexcept { return it; }

  reference operator*() const noexcept { return const_cast<reference>(it.operator*()); }
  pointer operator->() const noexcept { return const_cast<pointer>(it.operator->()); }

  mutable_iterator &operator++() noexcept {
    it.operator++();
    return *this;
  }
  mutable_iterator operator++(int) noexcept {
    auto temp = *this;
    it.operator++();
    return temp;
  }
  mutable_iterator &operator--() noexcept {
    it.operator--();
    return *this;
  }
  mutable_iterator operator--(int) noexcept {
    auto temp = *this;
    it.operator--();
    return temp;
  }

  mutable_iterator &operator+=(const difference_type d)
    requires random_access_iterator<const_iterator>
  {
    it += d;
    return *this;
  }

  mutable_iterator &operator-=(const difference_type d)
    requires random_access_iterator<const_iterator>
  {
    it.operator-=(d);
    return *this;
  }

  mutable_iterator operator+(const difference_type d) const
    requires random_access_iterator<const_iterator>
  {
    auto temp = *this;
    temp += d;
    return temp;
  }

  difference_type operator-(const mutable_iterator &rhs) const noexcept
    requires random_access_iterator<const_iterator>
  {
    return it - rhs.it;
  }

  mutable_iterator operator-(const difference_type d) const
    requires random_access_iterator<const_iterator>
  {
    auto temp = *this;
    temp -= d;
    return temp;
  }

  auto operator<=>(const mutable_iterator &) const noexcept = default;

private:
  const_iterator it;
};

template <class const_iterator> constexpr bool operator==(const_iterator a, mutable_iterator<const_iterator> b) noexcept {
  return static_cast<const_iterator>(b) == a;
}

template <class Iter> class reverse_iterator {
public:
  using iterator_type = Iter;
  using value_type = typename iterator_type::value_type;
  using difference_type = typename iterator_type::difference_type;
  using pointer = typename iterator_type::pointer;
  using reference = typename iterator_type::reference;

  reverse_iterator() = default;
  reverse_iterator(iterator_type a) : it(a) {}

  reference operator*() const noexcept {
    auto temp = it;
    return *(--temp);
  }
  pointer operator->() const noexcept {
    auto temp = it;
    return (--temp).operator->();
  }

  reverse_iterator &operator++() {
    --it;
    return *this;
  }
  reverse_iterator operator++(int) {
    auto temp = *this;
    --it;
    return temp;
  }

  reverse_iterator &operator--() {
    ++it;
    return *this;
  }

  reverse_iterator operator--(int) {
    auto temp = *this;
    ++it;
    return temp;
  }

  auto operator<=>(const reverse_iterator &) const noexcept = default;

private:
  iterator_type it;
};

template <bidirectional_iterator Iter> reverse_iterator<Iter> make_reverse_iterator(Iter i) { return reverse_iterator(i); }

template <class Iter> constexpr basic_const_iterator<Iter> make_const_iterator(Iter i) { return basic_const_iterator(i); }

class iterable {
public:
  constexpr auto cbegin(this auto const &self) noexcept { return self.begin(); }
  constexpr auto cend(this auto const &self) noexcept { return self.end(); }
  constexpr auto rbegin(this auto &&self) noexcept { return make_reverse_iterator(self.end()); }
  constexpr auto rend(this auto &&self) noexcept { return make_reverse_iterator(self.begin()); }
  constexpr auto crbegin(this auto const &self) noexcept { return self.rbegin(); }
  constexpr auto crend(this auto const &self) noexcept { return self.rend(); }
};

template <class T> class array_iterator {
public:
  using value_type = T;
  using pointer = T *;
  using reference = T &;
  using difference_type = ptrdiff_t;

  array_iterator() = default;
  array_iterator(T *p) : ptr(p) {}
  array_iterator(const T *p) : ptr(const_cast<pointer>(p)) {}

  reference operator*() const noexcept { return *ptr; }
  pointer operator->() const noexcept { return ptr; }

  array_iterator &operator++() noexcept {
    ++ptr;
    return *this;
  }
  array_iterator operator++(int) noexcept {
    auto temp = *this;
    ++ptr;
    return temp;
  }
  array_iterator &operator--() noexcept {
    --ptr;
    return *this;
  }
  array_iterator operator--(int) noexcept {
    auto temp = *this;
    --ptr;
    return temp;
  }

  array_iterator &operator+=(const difference_type d) noexcept {
    ptr += d;
    return *this;
  }

  array_iterator &operator-=(const difference_type d) noexcept {
    ptr -= d;
    return *this;
  }

  array_iterator operator+(const difference_type d) const noexcept {
    auto temp = *this;
    temp += d;
    return temp;
  }

  array_iterator operator-(const difference_type d) const noexcept {
    auto temp = *this;
    temp -= d;
    return temp;
  }

  difference_type operator-(array_iterator rhs) const noexcept { return ptr - rhs.ptr; }

  auto operator<=>(const array_iterator &) const noexcept = default;

private:
  pointer ptr = nullptr;
};

} // namespace aria