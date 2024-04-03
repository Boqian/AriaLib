#pragma once

namespace aria {

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


}