#pragma once
#include "binary_search_tree.h"
#include "stdexcept.h"

namespace aria {

template <class Key, class T, class Compare = less<Key>> class map : public binary_search_tree<Key, T, Compare> {
public:
  using Base = binary_search_tree<Key, T, Compare>;
  using key_type = Key;
  using value_type = typename Base::value_type;
  using mapped_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using key_compare = Compare;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  using Base::Base;

  T &operator[](const Key &key) {
    if (auto it = Base::find(key); it != Base::end())
      return it->second;
    else
      return Base::insert(value_type(key, T{})).first->second;
  }

  const T &at(const Key &key) const {
    if (auto it = Base::find(key); it == Base::end())
      throw out_of_range("aria::map::at() key is not found");
    else
      return it->second;
  }

  T &at(const Key &key) { return const_cast<T &>(as_const(*this).at(key)); }

  // template <class M> pair<iterator, bool> insert_or_assign(const Key &k, M &&obj) { auto res = insert(k, obj); }

private:
};

template <class Key, class T, class Compare> void swap(map<Key, T, Compare> &lhs, map<Key, T, Compare> &rhs) noexcept { lhs.swap(rhs); }

template <class Key, class T, class Compare, class Pred> size_t erase_if(map<Key, T, Compare> &c, Pred pred) {
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