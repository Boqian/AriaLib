#pragma once
#include "myhash.h"

namespace aria {

template <class Key, class T, class Hash = aria::hash<Key>, class KeyEqual = aria::equal_to<Key>>
class unordered_map : public my_hash<Key, T, Hash, KeyEqual> {
public:
  using Base = my_hash<Key, T, Hash, KeyEqual>;
  using key_type = Key;
  using value_type = typename Base::value_type;
  using mapped_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;

  using Base::Base;

  T &operator[](const Key &key) {
    auto it = Base::find(key);
    if (it != Base::end())
      return it->second;
    else
      return Base::insert(value_type(key, T{})).first->second;
  }

  template <class M> std::pair<iterator, bool> insert_or_assign(const Key &k, M &&obj) { auto res = insert(k, obj); }

private:
};

} // namespace aria