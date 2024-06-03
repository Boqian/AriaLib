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

node_base *next(node_base *p) {
  if (p->right) {
    p = p->right;
    while (p->left)
      p = p->left;
  } else {
    while (p->parent->right == p) {
      p = p->parent;
    }
    p = p->parent;
  }
  return p;
}

node_base *prev(node_base *p) {
  if (p->left) {
    p = p->left;
    while (p->right)
      p = p->right;
  } else {
    while (p->parent->left == p) {
      p = p->parent;
    }
    p = p->parent;
  }
  return p;
}

void link(node_base *parent, node_base *&pos, node_base *child) {
  pos = child;
  child->parent = parent;
}

// p and p->parent must exist
node_base *&parent_ref(node_base *p) { return (p == p->parent->left) ? p->parent->left : p->parent->right; }

void right_rotate(node_base *p) {
  if (!p->left)
    return;
  auto par = p->parent, l = p->left, lr = l->right;
  if (lr)
    link(p, p->left, lr);
  if (par)
    link(par, parent_ref(p), l);
  link(l, l->right, p);
}

void left_rotate(node_base *p) {
  if (!p->right)
    return;
  auto par = p->parent, r = p->right, rl = r->left;
  if (rl)
    link(p, p->right, rl);
  if (par)
    link(par, parent_ref(p), r);
  link(r, r->left, p);
}

void swap_parent(node_base *a, node_base *b) {
  auto pa = a->parent, pb = b->parent;
  bool is_pa_left = pa->left == a, is_pb_left = pb->left == b;
  a->parent = pb;
  b->parent = pa;
  (is_pa_left ? pa->left : pa->right) = b;
  (is_pb_left ? pb->left : pb->right) = a;
}

void swap_left(node_base *a, node_base *b) {
  auto la = a->left, lb = b->left;
  a->left = lb;
  b->left = la;
  if (la)
    la->parent = b;
  if (lb)
    lb->parent = a;
}

void swap_right(node_base *a, node_base *b) {
  auto ra = a->right, rb = b->right;
  a->right = rb;
  b->right = ra;
  if (ra)
    ra->parent = b;
  if (rb)
    rb->parent = a;
}

void swap(node_base &a, node_base &b) {
  if (a.parent == &b)
    return swap(b, a);

  const bool is_left = a.left == &b, is_right = a.right == &b;

  swap_parent(&a, &b);
  swap_left(&a, &b);
  swap_right(&a, &b);

  if (is_left) {
    a.parent = &b;
    b.left = &a;
  } else if (is_right) {
    a.parent = &b;
    b.right = &a;
  }
}

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
  using pointer = value_type *;
  using reference = value_type &;
  using difference_type = ptrdiff_t;
  using node_type = _bst::node<value_type>;
  using node_base_type = _bst::node_base;
  friend BSTType;

  bst_iterator() : ptr(nullptr) {}
  explicit bst_iterator(node_base_type *p) : ptr(p) {}
  explicit bst_iterator(const node_base_type *p) : ptr(const_cast<node_base_type *>(p)) {}

  reference operator*() const noexcept { return value(); }
  pointer operator->() const noexcept { return &value(); }

  bst_iterator &operator++() noexcept {
    ptr = next(ptr);
    return *this;
  }

  bst_iterator operator++(int) noexcept {
    auto temp = *this;
    operator++();
    return temp;
  }

  bst_iterator &operator--() noexcept {
    ptr = prev(ptr);
    return *this;
  }

  bst_iterator operator--(int) noexcept {
    auto temp = *this;
    operator--();
    return temp;
  }

  bool operator==(const bst_iterator &rhs) const noexcept { return ptr == rhs.ptr; }
  operator bool() const noexcept { return ptr; }

private:
  auto &value() const noexcept { return static_cast<node_type *>(ptr)->value; }
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

  binary_search_tree(initializer_list<value_type> init, const Compare &comp = Compare()) : m_compare(comp) {
    for (auto &x : init)
      insert(x);
  }

  ~binary_search_tree() { clear(); }

  binary_search_tree(const binary_search_tree &rhs) {
    for (auto &value : rhs)
      insert(value);
  }

  binary_search_tree(binary_search_tree &&rhs) noexcept { swap(rhs); }

  binary_search_tree &operator=(const binary_search_tree &rhs) {
    if (this != &rhs) {
      auto tmp = rhs;
      swap(tmp);
    }
    return *this;
  }

  binary_search_tree &operator=(binary_search_tree &&rhs) noexcept {
    if (this != &rhs) {
      auto tmp = move(rhs);
      swap(tmp);
    }
    return *this;
  }

  constexpr size_type size() const noexcept { return m_size; }
  constexpr bool empty() const noexcept { return m_size == 0; }

  void clear() noexcept {
    destroy_tree(root());
    adjust_on_empty();
  }

  bool operator==(const binary_search_tree &rhs) const noexcept {
    if (this == &rhs)
      return true;
    if (size() != rhs.size())
      return false;

    return equal(begin(), end(), rhs.begin());
  }

  void swap(binary_search_tree &rhs) noexcept {
    auto lhs_root = root();
    auto rhs_root = rhs.root();

    if (lhs_root)
      link(rhs.m_root_end, rhs.m_root_end->left, lhs_root);
    if (rhs_root)
      link(m_root_end, m_root_end->left, rhs_root);

    using aria::swap;
    swap(m_first, rhs.m_first);
    swap(m_size, rhs.m_size);
    swap(m_compare, rhs.m_compare);
    swap(m_alloc, rhs.m_alloc);

    adjust_on_empty();
    rhs.adjust_on_empty();
  }

  auto begin() const noexcept { return const_iterator(m_first); }
  auto end() const noexcept { return const_iterator(m_root_end); }
  auto begin() noexcept { return iterator(m_first); }
  auto end() noexcept { return iterator(m_root_end); }

  const_iterator find(const key_type &key) const { return const_iterator(find(root(), key)); }
  bool contains(const key_type &key) const { return find(key) != end(); }

  pair<iterator, bool> insert(const value_type &value) {
    auto [p, flag] = insert(m_root_end, value);
    return {iterator(p), flag};
  }

  iterator erase(iterator pos) {
    auto p = pos.ptr;
    auto res = next(pos);
    if (m_first == p)
      m_first = next(p);
    erase_node(p);
    return res;
  }

private:
  using node_type = _bst::node<value_type>;
  using node_base_type = _bst::node_base;

  template <class... Args> node_base_type *create_node(Args &&...args) {
    auto p = m_alloc.allocate(1);
    construct_at(p, forward<Args>(args)...);
    return p;
  }

  static const key_type &get_key(const node_base_type *p) { return _bst::get_key(static_cast<const node_type *>(p)->value); }
  bool compare(const key_type &key, const node_base_type *p) const { return (p == m_root_end) || m_compare(key, get_key(p)); }
  bool compare(const node_base_type *p, const key_type &key) const { return (p != m_root_end) && m_compare(get_key(p), key); }

  void destroy_node(node_base_type *p) {
    auto q = static_cast<node_type *>(p);
    destroy_at(q);
    m_alloc.deallocate(q, 1);
    m_size--;
  }

  void destroy_tree(node_base_type *p) {
    if (!p)
      return;
    destroy_tree(p->left);
    destroy_tree(p->right);
    destroy_node(p);
  }

  void adjust_on_empty() noexcept {
    if (m_size == 0) {
      m_root_end->left = nullptr;
      m_first = m_root_end;
    }
  }

  node_base_type *root() { return m_root_end->left; }
  const node_base_type *root() const { return m_root_end->left; }

  const node_base_type *find(const node_base_type *root, const key_type &key) const {
    if (!root)
      return m_root_end;
    if (compare(key, root)) {
      return find(root->left, key);
    } else if (compare(root, key)) {
      return find(root->right, key);
    } else {
      return root;
    }
  }

  void insert_at(node_base_type *parent, node_base_type *&pos, node_base_type *p) {
    if (&pos == &parent->left && m_first == parent) {
      m_first = p;
    }
    link(parent, pos, p);
    m_size++;
  }

  pair<node_base_type *, bool> insert(node_base_type *root, const_reference value) {
    if (compare(_bst::get_key(value), root)) {
      if (root->left) {
        return insert(root->left, value);
      } else {
        auto p = create_node(value);
        insert_at(root, root->left, p);
        return {p, true};
      }
    } else if (compare(root, _bst::get_key(value))) {
      if (root->right) {
        return insert(root->right, value);
      } else {
        auto p = create_node(value);
        insert_at(root, root->right, p);
        return {p, true};
      }
    } else {
      return {root, false};
    }
  }

  void erase_node(node_base_type *p) {
    if (!p->left && !p->right) {
      _bst::parent_ref(p) = nullptr;
      destroy_node(p);
    } else if (p->left && !p->right) {
      link(p->parent, _bst::parent_ref(p), p->left);
      destroy_node(p);
    } else if (p->right && !p->left) {
      link(p->parent, _bst::parent_ref(p), p->right);
      destroy_node(p);
    } else {
      auto prev_p = prev(p);
      _bst::swap(*prev_p, *p);
      erase_node(p);
    }
  }

  node_base_type m_root_node{};
  node_base_type *const m_root_end = &m_root_node;
  node_base_type *m_first = m_root_end;
  key_compare m_compare;
  size_type m_size{};
  allocator<node_type> m_alloc;
};

} // namespace aria