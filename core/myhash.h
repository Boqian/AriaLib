#pragma once

#include "functional.h"
#include "list.h"
#include "utility.h"
#include "vector.h"

namespace aria {

template <class Key, class T> struct _KeyVal {
  using type = pair<const Key, T>;
  const Key &key() const noexcept { return data.first; }
  T &value() const noexcept { return data.second; }
  type data;
};

template <class Key> struct _KeyVal<Key, Key> {
  using type = const Key;
  const Key &key() const noexcept { return data.first; }
  type data;
};

template <class Key, class T, class Hash = aria::hash<T>, class KeyEqual = aria::equal_to<Key>> class my_hash {
public:
  using key_type = Key;
  using value_type = _KeyVal<Key, T>;
  using mapped_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;

  my_hash() = default;

  size_type size() const noexcept { return m_list.size(); }
  bool empty() const noexcept { return m_list.empty(); }
  size_type bucket_count() const noexcept { return m_table.size(); }
  float load_factor() const { return empty() ? 1.0 : float(size()) / bucket_count(); }
  float max_load_factor() const noexcept { return m_max_load_factor; }

private:
  float m_max_load_factor = 1.0;
  list<value_type> m_list;
  using list_iter = list<value_type>::iterator;
  vector<list_iter> m_table;
};

} // namespace aria