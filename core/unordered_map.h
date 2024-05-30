#pragma once
#include "myhash.h"

namespace aria {

template <class Key, class T, class Hash = aria::hash<Key>, class KeyEqual = aria::equal_to<Key>>
class unordered_map : public hash_table<Key, T, Hash, KeyEqual> {
public:
  using Base = hash_table<Key, T, Hash, KeyEqual>;
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

  const T &at(const Key &key) const {
    auto it = Base::find(key);
    if (it == Base::end())
      throw std::out_of_range("unordered_map::at() key is not found");
    else
      return it->second;
  }

  T &at(const Key &key) { return const_cast<T &>(as_const(*this).at(key)); }

  template <class M> std::pair<iterator, bool> insert_or_assign(const Key &k, M &&obj) { auto res = insert(k, obj); }

private:
};

template <class Key, class T, class Hash, class KeyEqual>
void swap(unordered_map<Key, T, Hash, KeyEqual> &lhs, unordered_map<Key, T, Hash, KeyEqual> &rhs) noexcept {
  lhs.swap(rhs);
}

template <class Key, class T, class Hash, class KeyEqual>
bool operator==(const unordered_map<Key, T, Hash, KeyEqual> &lhs, const unordered_map<Key, T, Hash, KeyEqual> &rhs) {
  if (&lhs == &rhs)
    return true;
  if (lhs.size() != rhs.size())
    return false;
  return all_of(begin(lhs), end(lhs), [&](const auto &kv) {
    const auto &[k, v] = kv;
    auto it = rhs.find(k);
    return (it != rhs.end()) && (it->second == v);
  });
}

template <class Key, class T, class Hash, class KeyEqual, class Pred> size_t erase_if(unordered_map<Key, T, Hash, KeyEqual> &c, Pred pred) {
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