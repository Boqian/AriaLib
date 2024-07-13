#pragma once

#include "algorithm.h"
#include "functional.h"
#include "list.h"
#include "node_handle.h"
#include "utility.h"
#include "vector.h"
#include <cmath> //ceil

// hash_table is used to implement unordered_set and unordered_map

namespace aria {

namespace _hash_table {

template <class Key, class T> struct Traits {
  using value_type = pair<const Key, T>;
  using internal_node_type = _list::node<value_type>;
  using node_handle_type = node_handle<internal_node_type, _node_handle::map_base<Key, T>>;
  static const auto &get_key(const value_type &val) { return val.first; }
};

template <class Key> struct Traits<Key, void> {
  using value_type = Key;
  using internal_node_type = _list::node<value_type>;
  using node_handle_type = node_handle<internal_node_type, _node_handle::set_base<Key>>;
  static const auto &get_key(const value_type &val) { return val; }
};

} // namespace _hash_table

template <class Key, class T, class Hash = hash<Key>, class KeyEqual = equal_to<Key>> class hash_table : iterable {
public:
  using traits = _hash_table::Traits<Key, T>;
  using key_type = Key;
  using value_type = traits::value_type;
  using mapped_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using iterator = list<value_type>::iterator;
  using const_iterator = list<value_type>::const_iterator;
  using node_handle_type = traits::node_handle_type;

  hash_table() = default;
  ~hash_table() = default;

  explicit hash_table(size_type bucket_count) : m_table(bucket_count) {}

  hash_table(initializer_list<value_type> ilist) : m_table(ilist.size()) {
    for (auto &x : ilist)
      insert(x);
  }

  template <class InputIt> hash_table(InputIt first, InputIt last) : m_table(16) {
    for (; first != last; ++first)
      insert(*first);
  }

  hash_table(const hash_table &rhs)
      : m_max_load_factor(rhs.m_max_load_factor), m_list(rhs.m_list), m_table(rhs.m_table), m_hasher(rhs.m_hasher),
        m_key_equal(rhs.m_key_equal) {}

  hash_table(hash_table &&rhs) noexcept { swap(rhs); }

  hash_table &operator=(const hash_table &rhs) {
    if (&rhs != this) {
      auto temp = rhs;
      swap(temp);
    }
    return *this;
  }

  hash_table &operator=(hash_table &&rhs) noexcept {
    if (&rhs != this) {
      auto temp = aria::move(rhs);
      swap(temp);
    }
    return *this;
  }

  size_type size() const noexcept { return m_list.size(); }
  bool empty() const noexcept { return m_list.empty(); }
  size_type bucket_count() const noexcept { return m_table.size(); }
  float load_factor() const { return empty() ? 1.0 : float(size()) / bucket_count(); }
  float max_load_factor() const noexcept { return m_max_load_factor; }

  auto begin() const noexcept { return m_list.begin(); }
  auto end() const noexcept { return m_list.end(); }
  auto begin() noexcept { return m_list.begin(); }
  auto end() noexcept { return m_list.end(); }

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

    auto &bucket = get_bucket(traits::get_key(value));

    if (auto it = find(bucket, traits::get_key(value)); it != m_list.end())
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

    auto &bucket = get_bucket(traits::get_key(*pos));
    bucket.remove(pos);
    return m_list.erase(pos);
  }

  size_type erase(const key_type &key) {
    if (auto it = find(key); it != end()) {
      erase(it);
      return 1;
    }
    return 0;
  }

  void clear() noexcept { hash_table().swap(*this); }

  void swap(hash_table &rhs) noexcept {
    using aria::swap;
    swap(m_max_load_factor, rhs.m_max_load_factor);
    swap(m_list, rhs.m_list);
    swap(m_table, rhs.m_table);
    swap(m_hasher, rhs.m_hasher);
    swap(m_key_equal, rhs.m_key_equal);
  }

protected:
  struct bucket_type {
    bucket_type() = default;

    void add(iterator it) noexcept {
      first = it;
      size++;
    }

    void remove(iterator it) noexcept {
      if (first == it) {
        ++first;
      }
      --size;
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
      if (m_key_equal(traits::get_key(*it), key))
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