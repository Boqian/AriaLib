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

  list() noexcept = default;
  ~list() noexcept { clear(); }

  list(aria::initializer_list<value_type> init) {
    for (auto& value : init) {
      push_back(value);
    }
  }

  void push_back(value_type value) { add_node(m_last, aria::move(value)); }
  void push_front(value_type value) { add_node(nullptr, aria::move(value)); }
  void pop_back() noexcept { erase_node(m_last); }
  void pop_front() noexcept { erase_node(m_first); }

  reference front() noexcept { return m_first->value; } 
  const_reference front() const noexcept { return m_first->value; } 
  reference back() noexcept { return m_last->value; }
  const_reference back() const noexcept { return m_last->value; } 

  bool empty() const noexcept { return m_size == 0; }
  size_type size() const noexcept { return m_size; }

  void clear() noexcept {
    while (!empty())
      pop_back();
  }


  auto begin() const noexcept { return const_iterator(m_first, nullptr); }
  auto end() const noexcept { return const_iterator(nullptr, m_last); }
 // auto begin() noexcept { return iterator(get(0)); }
 // auto end() noexcept { return iterator(get(m_size)); }

  bool operator==(const list &rhs) const noexcept {
    if (this == &rhs)
      return true;
    if (size() != rhs.size())
      return false;
    for (auto p = m_first, q = rhs.m_first; p && q; p = p->next, q = q->next) {
      if (p->value != q->value)
        return false;
    }
    return true;
  }

private: 
  using node_type = node<T>;
  using node_allocator_type = typename Allocator:: template rebind_alloc<node_type>;

  node_type *create_node(value_type &&x) {
    auto p = m_alloc.allocate(1);
    aria::construct_at(p, forward<value_type>(x));
    return p;
  }

  void add_node(node_type *prev, node_type *p) {
    if (!prev) {
      p->next = m_first;
      m_first = p;
    } else {
      p->next = prev->next;
      p->prev = prev;
    }

    if (p->prev)
      p->prev->next = p;
    if (p->next)
      p->next->prev = p;

    if (prev == m_last)
      m_last = p;

    m_size++;
  }

  void add_node(node_type *prev, value_type &&x) { add_node(prev, create_node(forward<value_type>(x))); }

  void erase_node(node_type *p) {
    if (p == m_first) {
      m_first = p->next;
    } else if (p == m_last) {
      m_last = p->prev;
    } else {
      p->prev->next = p->next;
    }

    aria::destroy_at(p);
    m_alloc.deallocate(p, 1);
    m_size--;
  }

  node_type *m_first = nullptr;
  node_type *m_last = nullptr;
  size_type m_size = 0;
  node_allocator_type m_alloc;
};