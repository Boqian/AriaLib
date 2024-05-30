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
  explicit bst_iterator(const node_base_type *p) : ptr(p) {}

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

template <class Key, class T, class Compare = less<Key>> class binary_search_tree {
public:
  using key_type = Key;
  using value_type = typename _bst::KeyVal<Key, T>::type;
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

private:
  using node_type = _bst::node;
  using node_base_type = _bst::node_base;

  node_base_type **find(node_base_type **root, const_reference value) const {
    if (*root == nullptr || *root == m_end) {
      return root;
    }
    if (Compare(value, static_cast<node_type *>(root)->value)) {
      return find(*root->left, value);
    } else {
      return find(*root->right, value);
    }
  }

  node_base_type m_end_node;
  node_base_type *const m_end = &m_end_node;
  node_base_type *m_root = m_end;
  node_base_type *m_begin = m_end;
};

} // namespace aria