#pragma once
#include "allocator.h"
#include "utility.h"

template <class T> struct node {
  node() = default;

  node(T &&x, node *p = nullptr, node *n = nullptr) : value(forward<T>(x)), prev(p), next(n) {}

  T value;
  node *prev = nullptr;
  node *next = nullptr;
};

template <class ListType> class list_const_iterator {
public:
  using value_type = typename ListType::value_type;
  using pointer = typename ListType::const_pointer;
  using reference = const value_type &;
  using node_type = node<value_type>;

  list_const_iterator() = default;

  //store prev pointer to handle case like list::end() where p is null
  list_const_iterator(node_type *p, node_type *prev) : ptr(p), prev(prev) {}

  reference operator*() const noexcept { return ptr->value; }
  pointer operator->() const noexcept { return &(ptr->value); }

  list_const_iterator &operator++() noexcept {
    prev = ptr;
    ptr = ptr->next;
    return *this;
  }
  list_const_iterator operator++(int) noexcept {
    auto temp = *this;
    operator++();
    return temp;
  }
  list_const_iterator &operator--() noexcept {
    ptr = prev;
    prev = prev->prev;
    return *this;
  }
  list_const_iterator operator--(int) noexcept {
    auto temp = *this;
    operator--();
    return temp;
  }

  bool operator==(const list_const_iterator &rhs) const noexcept { return ptr == rhs.ptr; }

private:
  node_type *ptr = nullptr;
  node_type *prev = nullptr;
};

template <class T, class Allocator = allocator<T>> class list {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using allocator_type = Allocator;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
 // using iterator = vec_iterator<vector<T,Allocator>>;
  using const_iterator = list_const_iterator<list<T,Allocator>>;
 // using reverse_iterator = aria::reverse_iterator<iterator>;
 // using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  list() = default;

  void push_back(value_type &&value) {
    auto p = create_node(forward<value_type>(value));
    if (m_last) {
      m_last->next = p;
      m_last = p;
    } else {
      m_first = m_last = p;
    }
  }

  bool empty() const noexcept { 
      return m_size == 0;
  }

  auto begin() const noexcept { return const_iterator(m_first, nullptr); }
  auto end() const noexcept { return const_iterator(nullptr, m_last); }
 // auto begin() noexcept { return iterator(get(0)); }
 // auto end() noexcept { return iterator(get(m_size)); }

private: 
  using node_type = node<T>;
  using node_allocator_type = typename Allocator:: template rebind_alloc<node_type>;

  node_type *create_node(value_type&& x) {
    auto p = m_alloc.allocate(1);
    aria::construct_at(p, forward<value_type>(x));
    return p;
  }

  node_type *m_first = nullptr;
  node_type *m_last = nullptr;
  size_type m_size = 0;
  node_allocator_type m_alloc;
};