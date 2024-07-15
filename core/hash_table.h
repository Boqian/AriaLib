#pragma once

#include "algorithm.h"
#include "bit.h"
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

  void rehash(size_type bucket_count) {
    auto num_buckets = max<size_type>(bucket_count, std::ceil(size() / max_load_factor()));
    if (num_buckets > m_table.size())
      force_rehash(num_buckets);
  }

  void reserve(size_type count) { rehash(std::ceil(count / max_load_factor())); }

  pair<iterator, bool> insert(const_reference value) {
    reserve(size() + 1);
    const auto &key = traits::get_key(value);
    auto &bucket = get_bucket(key);
    if (auto it = find(bucket, key); it != m_list.end())
      return {it, false};

    auto insert_pos = m_list.insert(bucket.empty() ? m_list.end() : bucket.first(), value);
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

  node_handle_type extract(iterator pos) {
    if (pos == end())
      return {};
    get_bucket(traits::get_key(*pos)).remove(pos);
    return m_list.extract(pos);
  }

  node_handle_type extract(const key_type &key) { return extract(find(key)); }

  pair<iterator, bool> insert(node_handle_type &&nh) {
    if (!nh)
      return {end(), false};
    reserve(size() + 1);
    const auto &key = traits::get_key(nh->get_value());
    auto &bucket = get_bucket(key);
    if (auto it = find(bucket, key); it != m_list.end())
      return {it, false};

    auto insert_pos = m_list.insert(bucket.empty() ? m_list.end() : bucket.first(), move(nh));
    bucket.add(insert_pos);
    return {insert_pos, true};
  }

protected:
  class bucket_type {
  public:
    void add(iterator it) noexcept {
      m_first = it;
      m_size++;
    }
    void remove(iterator it) noexcept {
      if (m_first == it)
        ++m_first;
      --m_size;
    }
    bool empty() const noexcept { return m_size == 0; }
    iterator first() const noexcept { return m_first; }
    size_t size() const noexcept { return m_size; }

  private:
    iterator m_first{};
    size_t m_size{};
  };

  size_type bucket_index(const key_type &key) const noexcept { return m_hasher(key) % bucket_count(); }
  bucket_type &get_bucket(const key_type &key) noexcept { return m_table[bucket_index(key)]; }

  iterator find(const bucket_type &bucket, const key_type &key) const {
    auto it = bucket.first();
    for (int j = 0; j < bucket.size(); ++j, ++it) {
      if (m_key_equal(traits::get_key(*it), key))
        return it;
    }
    return m_list.end();
  }

  void force_rehash(size_t bucket_count) {
    bucket_count = bit_ceil(bucket_count); // always power of 2
    m_table = vector<bucket_type>(bucket_count);
    auto input = move(m_list);
    while (!input.empty())
      insert(input.extract(input.begin()));
  }

  float m_max_load_factor = 1.0;
  list<value_type> m_list;
  vector<bucket_type> m_table;
  hasher m_hasher;
  key_equal m_key_equal;
};

} // namespace aria