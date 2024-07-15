#pragma once

#include "algorithm.h"
#include "bit.h"
#include "functional.h"
#include "list.h"
#include "node_handle.h"
#include "utility.h"
#include "vector.h"
#include <cassert>
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

template <class Key, class T, class Hash = hash<Key>, class KeyEqual = equal_to<Key>> class hash_table : iterable_mixin {
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

  explicit hash_table(size_type bucket_count) : m_buckets(bucket_count) {}

  hash_table(initializer_list<value_type> ilist) : m_buckets(ilist.size()) {
    for (auto &x : ilist)
      insert(x);
  }

  template <class InputIt> hash_table(InputIt first, InputIt last) : m_buckets(16) {
    for (; first != last; ++first)
      insert(*first);
  }

  hash_table(const hash_table &rhs)
      : m_max_load_factor(rhs.m_max_load_factor), m_list(rhs.m_list), m_buckets(rhs.m_buckets), m_hasher(rhs.m_hasher),
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

  //--------------------  Iterators (cbegin and rebegin are derived from iterable_mixin )--------------------
  auto begin() const noexcept { return m_list.begin(); }
  auto end() const noexcept { return m_list.end(); }
  auto begin() noexcept { return m_list.begin(); }
  auto end() noexcept { return m_list.end(); }

  //--------------------  Capacity--------------------
  size_type size() const noexcept { return m_list.size(); }
  bool empty() const noexcept { return m_list.empty(); }

  //--------------------  Lookup--------------------

  iterator find(const key_type &key) { return as_const(*this).find(key); }
  const_iterator find(const key_type &key) const { return m_buckets.empty() ? end() : find(get_bucket(key), key); }
  bool contains(const key_type &key) const { return find(key) != m_list.end(); }

  //--------------------  Bucket interface--------------------
  size_type bucket_count() const noexcept { return m_buckets.size(); }
  size_type bucket(const Key &key) const { return m_hasher(key) % bucket_count(); }

  //--------------------Hash policy--------------------
  void rehash(size_type bucket_count) {
    auto num_buckets = max<size_type>(bucket_count, std::ceil(size() / max_load_factor()));
    if (num_buckets > m_buckets.size())
      force_rehash(num_buckets);
  }

  void reserve(size_type count) { rehash(std::ceil(count / max_load_factor())); }
  float load_factor() const noexcept { return empty() ? 1.0 : float(size()) / bucket_count(); }
  float max_load_factor() const noexcept { return m_max_load_factor; }
  void max_load_factor(float ml) noexcept { m_max_load_factor = ml; }

  //--------------------Modifiers--------------------
  pair<iterator, bool> insert(const_reference value) {
    auto [bucket_ptr, insert_pos] = find_insert_position(value);
    if (!bucket_ptr)
      return {insert_pos, false};

    insert_pos = m_list.insert(insert_pos, value);
    bucket_ptr->add(insert_pos);
    return {insert_pos, true};
  }

  pair<iterator, bool> insert(node_handle_type &&nh) {
    if (!nh)
      return {end(), false};
    auto [bucket_ptr, insert_pos] = find_insert_position(nh->get_value());
    if (!bucket_ptr)
      return {insert_pos, false};

    insert_pos = m_list.insert(insert_pos, move(nh));
    bucket_ptr->add(insert_pos);
    return {insert_pos, true};
  }

  template <class... Args> pair<iterator, bool> emplace(Args &&...args) {
    return insert(node_handle_type(m_list.create_node(forward<Args>(args)...)));
  }

  iterator erase(const_iterator pos) {
    if (pos == m_list.end())
      return pos;

    get_bucket(traits::get_key(*pos)).remove(pos);
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
    swap(m_buckets, rhs.m_buckets);
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

protected:
  class bucket_type {
  public:
    void add(iterator it) noexcept {
      m_first = it;
      ++m_size;
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

  template <class Self> decltype(auto) get_bucket(this Self &&self, const key_type &key) {
    assert(!self.m_buckets.empty());
    return self.m_buckets[self.bucket(key)];
  }

  const_iterator find(const bucket_type &bucket, const key_type &key) const {
    auto it = bucket.first();
    for (int j = 0; j < bucket.size(); ++j, ++it) {
      if (m_key_equal(traits::get_key(*it), key))
        return it;
    }
    return m_list.end();
  }

  pair<bucket_type *, iterator> find_insert_position(const_reference value) {
    reserve(size() + 1);
    const auto &key = traits::get_key(value);
    auto &bucket = get_bucket(key);
    if (auto it = find(bucket, key); it != end()) // already inserted
      return {nullptr, it};
    else
      return {&bucket, bucket.empty() ? end() : bucket.first()};
  }

  void force_rehash(size_t bucket_count) {
    bucket_count = bit_ceil(bucket_count); // always power of 2
    m_buckets = vector<bucket_type>(bucket_count);
    auto input = move(m_list);
    while (!input.empty())
      insert(input.extract(input.begin()));
  }

  float m_max_load_factor = 1.0;
  list<value_type> m_list;
  vector<bucket_type> m_buckets;
  hasher m_hasher;
  key_equal m_key_equal;
};

} // namespace aria