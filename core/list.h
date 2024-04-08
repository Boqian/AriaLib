#pragma once
#include "allocator.h"
#include "iterator.h"
#include "utility.h"

struct node_base {
  node_base *prev = nullptr;
  node_base *next = nullptr;
};

template <class T> struct node : public node_base {
  node() = default;
  node(T &&x) : value(forward<T>(x)) {}
  node_base base;
  T value;
};

template <class ListType> class list_const_iterator {
public:
  using value_type = typename ListType::value_type;
  using pointer = typename ListType::const_pointer;
  using reference = const value_type &;
  using difference_type = std::ptrdiff_t;
  using node_type = node<value_type>;
  friend ListType;

  list_const_iterator() = default;

  list_const_iterator(const node_base *p) : ptr(p) {}

  reference operator*() const noexcept { return value(); }
  pointer operator->() const noexcept { return &value(); }

  list_const_iterator &operator++() noexcept {
    ptr = ptr->next;
    return *this;
  }
  list_const_iterator operator++(int) noexcept {
    auto temp = *this;
    operator++();
    return temp;
  }
  list_const_iterator &operator--() noexcept {
    ptr = ptr->prev;
    return *this;
  }
  list_const_iterator operator--(int) noexcept {
    auto temp = *this;
    operator--();
    return temp;
  }

  bool operator==(const list_const_iterator &rhs) const noexcept { return ptr == rhs.ptr; }

private:
  reference value() const noexcept { return static_cast<const node_type *>(ptr)->value; }
  const node_base *ptr;
};

template <class ListType> class list_iterator : public list_const_iterator<ListType> {
  using Base = list_const_iterator<ListType>;

public:
  using value_type = typename ListType::value_type;
  using pointer = typename ListType::pointer;
  using reference = value_type &;
  using difference_type = std::ptrdiff_t;
  using Base::Base;

  reference operator*() const noexcept { return const_cast<reference>(Base::operator*()); }
  pointer operator->() const noexcept { return const_cast<pointer>(Base::operator->()); }

  list_iterator &operator++() noexcept {
    Base::operator++();
    return *this;
  }
  list_iterator operator++(int) noexcept {
    auto temp = *this;
    Base::operator++();
    return temp;
  }
  list_iterator &operator--() noexcept {
    Base::operator--();
    return *this;
  }
  list_iterator operator--(int) noexcept {
    auto temp = *this;
    Base::operator--();
    return temp;
  }
};

template <class T, class Allocator = allocator<T>> class list {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using allocator_type = Allocator;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using iterator = list_iterator<list<T, Allocator>>;
  using const_iterator = list_const_iterator<list<T, Allocator>>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  list() noexcept = default;
  ~list() noexcept { clear(); }

  list(aria::initializer_list<value_type> init) {
    for (auto &value : init) {
      push_back(value);
    }
  }

  void push_back(value_type value) { insert_node(&m_end, aria::move(value)); }
  void push_front(value_type value) { insert_node(m_first, aria::move(value)); }
  void pop_back() noexcept { erase_node(last()); }
  void pop_front() noexcept { erase_node(m_first); }

  reference front() noexcept { return first()->value; }
  const_reference front() const noexcept { return first()->value; }
  reference back() noexcept { return last()->value; }
  const_reference back() const noexcept { return last()->value; }

  bool empty() const noexcept { return m_size == 0; }
  size_type size() const noexcept { return m_size; }

  void clear() noexcept {
    while (!empty())
      pop_back();
  }

  auto begin() const noexcept { return const_iterator(m_first); }
  auto end() const noexcept { return const_iterator(&m_end); }
  auto begin() noexcept { return iterator(m_first); }
  auto end() noexcept { return iterator(&m_end); }
  auto rbegin() const noexcept { return const_reverse_iterator(end()); }
  auto rend() const noexcept { return const_reverse_iterator(begin()); }
  auto rbegin() noexcept { return reverse_iterator(end()); }
  auto rend() noexcept { return reverse_iterator(begin()); }

  iterator insert(const_iterator pos, T &&value) { return insert_node(get_ptr(pos), forward<T>(value)); }

  iterator erase(const_iterator pos) { return erase_node(get_ptr(pos)); }

  bool operator==(const list &rhs) const noexcept {
    if (this == &rhs)
      return true;
    if (size() != rhs.size())
      return false;
    if (size() == 0)
      return true;
    for (auto p = m_first, q = rhs.m_first; p != &m_end; p = p->next, q = q->next) {
      if (cast(p)->value != cast(q)->value)
        return false;
    }
    return true;
  }

private:
  using node_type = node<T>;
  using node_allocator_type = typename Allocator::template rebind_alloc<node_type>;

  node_type *cast(node_base *p) const noexcept { return static_cast<node_type *>(p); }
  node_type *last() const noexcept { return cast(m_end.prev); }
  node_type *first() const noexcept { return cast(m_first); }
  static node_base *get_ptr(const_iterator pos) noexcept { return const_cast<node_base *>(pos.ptr); }

  static void link(node_base *&first, node_base *&second) noexcept {
    first->next = second;
    second->prev = first;
  }

  node_type *create_node(value_type &&x) {
    auto p = m_alloc.allocate(1);
    aria::construct_at(p, forward<value_type>(x));
    return p;
  }

  node_base *insert_node(node_base *pos, node_base *p) {
    auto prev = pos->prev;
    link(p, pos);
    if (prev) {
      link(prev, p);
    } else {
      m_first = p;
    }
    m_size++;
    return p;
  }

  node_base *insert_node(node_base *pos, value_type &&x) { return insert_node(pos, create_node(forward<value_type>(x))); }

  node_base *erase_node(node_base *p) {
    if (p == &m_end)
      return p;

    if (p == m_first) {
      m_first = p->next;
    } else {
      link(p->prev, p->next);
    }

    auto res = p->next;
    aria::destroy_at(cast(p));
    m_alloc.deallocate(cast(p), 1);
    m_size--;
    return res;
  }

  node_base m_end;
  node_base *m_first = &m_end;
  size_type m_size = 0;
  node_allocator_type m_alloc;
};