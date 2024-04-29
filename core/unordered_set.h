#pragma once
#include "myhash.h"

namespace aria {

template <class Key, class Hash = aria::hash<Key>, class KeyEqual = aria::equal_to<Key>>
class unordered_set : public my_hash<Key, void, Hash, KeyEqual> {
public:
  using Base = my_hash<Key, void, Hash, KeyEqual>;
  using key_type = Key;
  using value_type = typename Base::value_type;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  static_assert(is_same_v<key_type, value_type>);

  using Base::Base;

private:
};

} // namespace aria