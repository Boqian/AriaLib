#pragma once
#include "binary_search_tree.h"

namespace aria {

template <class Key, class Compare = less<Key>> class set : public binary_search_tree<Key, void, Compare> {
public:
  using Base = binary_search_tree<Key, void, Compare>;
  using key_type = Key;
  using value_type = Key;
  using pointer = value_type *;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using key_compare = Compare;
  using value_compare = Compare;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  using Base::Base;
};

template <class Key, class Compare> void swap(set<Key, Compare> &lhs, set<Key, Compare> &rhs) noexcept { lhs.swap(rhs); }

template <class Key, class Compare, class Pred> size_t erase_if(set<Key, Compare> &c, Pred pred) {
  size_t old_size = c.size();
  for (auto it = begin(c); it != end(c);) {
    if (pred(*it)) {
      it = c.erase(it);
    } else
      ++it;
  }
  return old_size - c.size();
}

} // namespace aria
