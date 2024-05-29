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

template <class Key, class Hash, class KeyEqual>
void swap(unordered_set<Key, Hash, KeyEqual> &lhs, unordered_set<Key, Hash, KeyEqual> &rhs) noexcept {
  lhs.swap(rhs);
}

template <class Key, class Hash, class KeyEqual>
bool operator==(const unordered_set<Key, Hash, KeyEqual> &lhs, const unordered_set<Key, Hash, KeyEqual> &rhs) {
  if (&lhs == &rhs)
    return true;
  if (lhs.size() != rhs.size())
    return false;
  return all_of(begin(lhs), end(lhs), [&](const auto &key) { return rhs.contains(key); });
}

} // namespace aria