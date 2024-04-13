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
  using iterator = list<value_type>::iterator;
  using const_iterator = list<value_type>::const_iterator;

  T &operator[](const Key &key) {
    auto it = Base::find(key);
    return it->second;
  }

  template <class M> std::pair<iterator, bool> insert_or_assign(const Key &k, M &&obj) {}

private:
};

} // namespace aria