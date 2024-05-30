#pragma once
#include "algorithm.h"
#include "allocator.h"
#include "iterator.h"
#include "utility.h"

namespace aria {

namespace _bst {
struct node_base {
  node_base *parent{};
  node_base *left{};
  node_base *right{};
};

template <class T> struct node : public node_base {
  template <class... Args>
    requires is_constructible_v<T, Args...>
  node(Args &&...args) : value(forward<Args>(args)...) {}
  T value;
};

template <class Key, class T> struct KeyVal {
  using type = pair<const Key, T>;
};

template <class Key> struct KeyVal<Key, void> {
  using type = Key;
};

template <class T> const auto &get_key(const T &x) {
  if constexpr (is_pair_v<T>) {
    return x.first;
  } else
    return x;
}
} // namespace _bst

template <class BSTType> class bst_iterator {
public:
  using value_type = typename BSTType::value_type;
  using pointer = typename BSTType::pointer;
  using reference = value_type &;
  using difference_type = ptrdiff_t;
  using node_type = _bst::node<value_type>;
  using node_base_type = _bst::node_base;
  friend BSTType;

  bst_iterator() : ptr(nullptr) {}
  explicit bst_iterator(node_base_type *p) : ptr(p) {}

  reference operator*() const noexcept { return value(); }
  pointer operator->() const noexcept { return &value(); }

  bst_iterator &operator++() noexcept { return *this; }
  bst_iterator operator++(int) noexcept {
    auto temp = *this;
    operator++();
    return temp;
  }
  bst_iterator &operator--() noexcept { return *this; }
  bst_iterator operator--(int) noexcept {
    auto temp = *this;
    operator--();
    return temp;
  }

  bool operator==(const bst_iterator &rhs) const noexcept { return ptr == rhs.ptr; }

  operator bool() const noexcept { return ptr; }

private:
  reference value() const noexcept { return static_cast<const node_type *>(ptr)->value; }
  node_base_type *ptr;
};

template <class Key, class T, class Compare = less<Key>> class binary_search_tree : public iterable {
public:
  using key_type = Key;
  using value_type = typename _bst::KeyVal<Key, T>::type;
  using pointer = value_type *;
  using mapped_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using key_compare = Compare;
  using iterator = bst_iterator<binary_search_tree<Key, T, Compare>>;
  using const_iterator = basic_const_iterator<iterator>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  binary_search_tree() = default;

  ~binary_search_tree() {}

  binary_search_tree(const binary_search_tree &rhs) {}

  void swap(binary_search_tree &rhs) noexcept {}

  auto begin() const noexcept { return const_iterator(m_first); }
  auto end() const noexcept { return const_iterator(m_end); }
  auto begin() noexcept { return iterator(m_first); }
  auto end() noexcept { return iterator(m_end); }

  const_iterator find(const key_type &key) {
    auto p = *find(&m_root, key);
    if (p == nullptr)
      p = m_end;
    return const_iterator(p);
  }

private:
  using node_type = _bst::node<value_type>;
  using node_base_type = _bst::node_base;

  static const key_type &get_key(const node_base_type *p) { return _bst::get_key(static_cast<const node_type *>(p)->value); }

  auto find(node_base_type **root, const key_type &key) {
    if (*root == nullptr || *root == m_end) {
      return root;
    }
    if (m_compare(key, get_key(*root))) {
      return find(&(*root)->left, key);
    } else {
      return find(&(*root)->right, key);
    }
  }

  node_base_type m_end_node;
  node_base_type *const m_end = &m_end_node;
  node_base_type *m_root = m_end;
  node_base_type *m_first = m_end;
  key_compare m_compare;
};

} // namespace aria