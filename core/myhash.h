#pragma once

#include "functional.h"
#include "list.h"
#include "utility.h"
#include "vector.h"
#include <algorithm.h>
#include <cassert>
// #include <list>

namespace aria {

template <class Key, class T> struct _KeyVal {
  using value_type = pair<const Key, T>;

  template <class... Args> _KeyVal(Args &&...args) : data(forward<Args>(args)...) {}

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
  using iterator = list<value_type>::iterator;
  using const_iterator = list<value_type>::const_iterator;
  my_hash() = default;

  size_type size() const noexcept { return m_list.size(); }
  bool empty() const noexcept { return m_list.empty(); }
  size_type bucket_count() const noexcept { return m_table.size(); }
  float load_factor() const { return empty() ? 1.0 : float(size()) / bucket_count(); }
  float max_load_factor() const noexcept { return m_max_load_factor; }

  auto begin() const noexcept { return m_list.begin(); }
  auto end() const noexcept { return m_list.end(); }
  auto begin() noexcept { return m_list.begin(); }
  auto end() noexcept { return m_list.end(); }
  auto rbegin() const noexcept { return m_list.rbegin(); }
  auto rend() const noexcept { return m_list.rend(); }
  auto rbegin() noexcept { return m_list.rbegin(); }
  auto rend() noexcept { return m_list.rend(); }

  void reserve(size_type n) {
    if (n <= bucket_count())
      return;
    m_table = vector<bucket_type>(n);
    auto input = move(m_list);
    for (auto &val : input)
      insert(val);
  }

  pair<iterator, bool> insert(const_reference value) {
    resize_if_needed(1);

    auto &bucket = get_bucket(value.key());

    if (auto it = find(bucket, value.key()); it != m_list.end())
      return {it, false};

    auto insert_pos = m_list.insert(bucket ? bucket.first : m_list.end(), value);
    bucket.add(insert_pos);
    return {insert_pos, true};
  }

  iterator find(const key_type &key) {
    const auto &bucket = m_table[bucket_index(key)];
    return find(bucket, key);
  }

  const_iterator find(const key_type &key) const {
    const auto &bucket = m_table[bucket_index(key)];
    return find(bucket, key);
  }

  bool contains(const key_type &key) const { return find(key) != m_list.end(); }

  iterator erase(const_iterator pos) {
    if (pos == m_list.end())
      return pos;

    auto &bucket = get_bucket(pos->key());
    iterator res;
    if (bucket.first == pos) {
      res = bucket.first = m_list.erase(pos);
    } else {
      res = m_list.erase(pos);
    }
    bucket.size--;
    return res;
  }

  size_type erase(const key_type &key) {
    auto it = find(key);
    if (it != end()) {
      erase(it);
      return 1;
    }
    return 0;
  }

protected:
  struct bucket_type {
    bucket_type() = default;
    void add(iterator it) {
      first = it;
      size++;
    }
    operator bool() const noexcept { return size > 0; }
    iterator first{};
    size_t size{};
  };

  size_type bucket_index(const key_type &key) const noexcept { return m_hasher(key) % bucket_count(); }
  bucket_type &get_bucket(const key_type &key) noexcept { return m_table[bucket_index(key)]; }

  iterator find(const bucket_type &bucket, const key_type &key) const {
    auto it = bucket.first;
    for (int j = 0; j < bucket.size; j++, it++) {
      assert(m_hasher(key) % bucket_count() == m_hasher(it->key()) % bucket_count());
      if (m_key_equal(it->key(), key))
        return it;
    }
    return m_list.end();
  }

  void resize_if_needed(size_type added_size) {
    const size_type new_size = size() + added_size;
    const size_type num_bucket_needed = ceil(m_max_load_factor * new_size);
    if (bucket_count() < num_bucket_needed)
      reserve(num_bucket_needed * 2);
  }

  float m_max_load_factor = 1.0;
  list<value_type> m_list;
  vector<bucket_type> m_table;
  hasher m_hasher;
  key_equal m_key_equal;
};

} // namespace aria