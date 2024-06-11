#pragma once
#include "list.h"
#include "optional.h"
#include "unordered_map.h"

namespace aria {

template <class Key, class T> class LRUCache {
public:
  explicit LRUCache(size_t capacity) : m_capacity(capacity) { m_map.reserve(m_capacity * 2); }

  void update(const Key &key, const T &value) {
    if (auto it = m_map.find(key); it != m_map.end()) {
      move_node_to_front(it->second);
      it->second->second = value;
    } else {
      if (m_capacity == m_list.size())
        erase_lru();
      m_list.push_front(pair(key, value));
      m_map[key] = m_list.begin();
    }
  }

  optional<T> get(const Key &key) {
    auto it = m_map.find(key);
    if (it == m_map.end())
      return {};

    move_node_to_front(it->second);
    return it->second->second;
  }

private:
  using iter = list<pair<Key, T>>::iterator;

  void move_node_to_front(iter it) { m_list.splice(m_list.begin(), m_list, it); }

  void erase_lru() {
    auto &key = m_list.rbegin()->first;
    m_map.erase(key);
    m_list.pop_back();
  }

  const size_t m_capacity;
  list<pair<Key, T>> m_list;
  unordered_map<Key, iter> m_map;
};

} // namespace aria