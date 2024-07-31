#pragma once
#include "vector.h"

namespace aria {

class DisjointSet {
public:
  DisjointSet(size_t n) : m_root(n) {
    for (auto i = 0u; i < n; i++)
      m_root[i] = i;
  }

  size_t get_root(size_t i) {
    if (m_root[i] != i)
      m_root[i] = get_root(m_root[i]);
    return m_root[i];
  }

  void connect(size_t i, size_t j) { m_root[get_root(i)] = get_root(j); }

private:
  vector<int> m_root;
};

} // namespace aria