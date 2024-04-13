#pragma once

#include "functional.h"
#include "list.h"
#include "utility.h"
#include "vector.h"

namespace aria {

template <class Key, class T> struct _KeyVal {
  using value_type = pair<const Key, T>;
  _KeyVal() = default;
  _KeyVal(const value_type &a) : data(a) {}
  _KeyVal(value_type &&a) : data(aria::move(a)) {}
  const Key &key() const noexcept { return data.first; }
  T &value() const noexcept { return data.second; }
  value_type data;
};

template <class Key> struct _KeyVal<Key, void> {
  using value_type = const Key;
  _KeyVal() = default;
  _KeyVal(const value_type &a) : data(a) {}
  _KeyVal(value_type &&a) : data(aria::move(a)) {}
  const Key &key() const noexcept { return data; }
  value_type data;
};

template <class Key, class T, class Hash = aria::hash<Key>, class KeyEqual = aria::equal_to<Key>> class my_hash {
public:
  using key_type = Key;
  using value_type = typename _KeyVal<Key, T>;
  using mapped_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using list_iter = list<value_type>::iterator;

  my_hash() = default;

  size_type size() const noexcept { return m_list.size(); }
  bool empty() const noexcept { return m_list.empty(); }
  size_type bucket_count() const noexcept { return m_table.size(); }
  float load_factor() const { return empty() ? 1.0 : float(size()) / bucket_count(); }
  float max_load_factor() const noexcept { return m_max_load_factor; }

  void reserve(size_type n) {
    if (n <= bucket_count())
      return;

    m_table = vector<bucket_type>(n);
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
      auto &bucket = m_table[bucket_index(it->key())];
      bucket.size++;
      if (!bucket.first)
        bucket.first = it;
    }
  }

  list_iter insert(const_reference value) {
    if (bucket_count() == 0)
      reserve(10);

    auto &bucket = m_table[bucket_index(value.key())];
    if (find(value.key(), bucket))
      return {};

    auto inserted_pos = m_list.insert(bucket.first ? bucket.first : m_list.end(), value);
    bucket.add(inserted_pos);
  }

  list_iter find(const key_type &key) const {
    const auto &bucket = m_table[bucket_index(key)];
    return find(key, bucket);
  }

  list_iter erase(list_iter pos) {
    if (pos == m_list.end())
      return pos;

    const auto &bucket = m_table[bucket_index(pos->key())];
    list_iter res;
    if (bucket.first == pos) {
      bucket.first = res = m_list.erase(pos);
    } else {
      res = m_list.erase(pos);
    }
    bucket.size--;
    return res;
  }

protected:
  struct bucket_type {
    bucket_type() = default;
    void add(list_iter it) {
      first = it;
      size++;
    }

    bucket_type(list_iter it) : first(it), size(1) {}
    operator bool() const noexcept { return size == 0; }
    list_iter first{};
    size_t size{};
  };

  size_type bucket_index(const key_type &key) const noexcept { return m_hasher(key) % bucket_count(); }

  list_iter find(const key_type &key, const bucket_type &bucket) const {
    auto it = bucket.first;
    for (int j = 0; j < bucket.size; j++, it++) {
      if (m_key_equal(it->key(), key))
        return it;
    }
    return {};
  }

  float m_max_load_factor = 1.0;
  list<value_type> m_list;

  vector<bucket_type> m_table;
  hasher m_hasher;
  key_equal m_key_equal;
};

} // namespace aria