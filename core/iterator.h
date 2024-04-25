#pragma once

#include "utility.h"

namespace aria {

template <class I>
concept forward_iterator = requires(I i) {
  { i++ } -> same_as<I>;
  { ++i } -> same_as<I &>;
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

template <class Container> auto begin(Container &cont) { return cont.begin(); }
template <class Container> auto begin(const Container &cont) { return cont.begin(); }
template <class Container> auto cbegin(const Container &cont) { return cont.begin(); }
template <class Container> auto end(Container &cont) { return cont.end(); }
template <class Container> auto end(const Container &cont) { return cont.end(); }
template <class Container> auto cend(const Container &cont) { return cont.end(); }
template <class Container> auto rbegin(Container &cont) { return cont.rbegin(); }
template <class Container> auto rbegin(const Container &cont) { return cont.rbegin(); }
template <class Container> auto crbegin(const Container &cont) { return cont.rbegin(); }
template <class Container> auto rend(Container &cont) { return cont.rend(); }
template <class Container> auto rend(const Container &cont) { return cont.rend(); }
template <class Container> auto crend(const Container &cont) { return cont.rend(); }

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

} // namespace aria