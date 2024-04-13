#pragma once
#include "allocator.h"
#include "iterator.h"
#include "utility.h"

struct _node_base {
  _node_base *prev = nullptr;
  _node_base *next = nullptr;
};

template <class T> struct node : public _node_base {
  node() = default;
  node(T &&x) : value(forward<T>(x)) {}
  _node_base base;
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

  list_const_iterator() : ptr(nullptr) {}
  explicit list_const_iterator(const _node_base *p) : ptr(p) {}

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

  operator bool() const noexcept { return ptr; }

private:
  reference value() const noexcept { return static_cast<const node_type *>(ptr)->value; }
  const _node_base *ptr;
};

template <class ListType> class list_iterator : public list_const_iterator<ListType> {
  using Base = list_const_iterator<ListType>;

public:
  using value_type = typename ListType::value_type;
  using pointer = typename ListType::pointer;
  using reference = value_type &;
  using difference_type = std::ptrdiff_t;
  using Base::Base;

  list_iterator(Base base) : Base(base) {}
  reference operator*() const noexcept { return const_cast<reference>(Base::operator*()); }
  pointer operator->() const noexcept { return const_cast<pointer>(Base::operator->()); }
  operator Base() const noexcept { return *this; }

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

template <class T, class Allocator = aria::allocator<T>> class list {
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

  list(const list &rhs) {
    for (auto &x : rhs)
      push_back(x);
  }

  list &operator=(const list &rhs) {
    if (this != &rhs) {
      auto temp = rhs;
      swap(temp);
    }
    return *this;
  }

  list(list &&rhs) noexcept { swap(rhs); }

  list &operator=(list &&rhs) noexcept {
    auto temp = aria::move(rhs);
    swap(temp);
    return *this;
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

  iterator insert(const_iterator pos, value_type value) { return iterator(insert_node(get_ptr(pos), aria::move(value))); }
  iterator erase(const_iterator pos) { return iterator(erase_node(get_ptr(pos))); }

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

  void swap(list &rhs) noexcept {
    auto lhs_last = m_end.prev;
    auto rhs_last = rhs.m_end.prev;
    if (lhs_last)
      link(*lhs_last, rhs.m_end);
    if (rhs_last)
      link(*rhs_last, m_end);

    aria::swap(m_first, rhs.m_first);
    aria::swap(m_size, rhs.m_size);
    aria::swap(m_alloc, rhs.m_alloc);

    adjust_on_empty();
    rhs.adjust_on_empty();
  }

private:
  using node_type = node<T>;
  using node_allocator_type = typename Allocator::template rebind_alloc<node_type>;

  node_type *cast(_node_base *p) const noexcept { return static_cast<node_type *>(p); }
  node_type *last() const noexcept { return cast(m_end.prev); }
  node_type *first() const noexcept { return cast(m_first); }
  static _node_base *get_ptr(const_iterator pos) noexcept { return const_cast<_node_base *>(pos.ptr); }

  static void link(_node_base &first, _node_base &second) noexcept {
    first.next = &second;
    second.prev = &first;
  }

  node_type *create_node(value_type &&x) {
    auto p = m_alloc.allocate(1);
    aria::construct_at(p, forward<value_type>(x));
    return p;
  }

  _node_base *insert_node(_node_base *pos, _node_base *p) {
    auto prev = pos->prev;
    link(*p, *pos);
    if (prev) {
      link(*prev, *p);
    } else {
      m_first = p;
    }
    m_size++;
    return p;
  }

  _node_base *insert_node(_node_base *pos, value_type &&x) { return insert_node(pos, create_node(forward<value_type>(x))); }
  _node_base *insert_node(_node_base *pos, const value_type &x) { return insert_node(pos, create_node(forward<value_type>(x))); }

  _node_base *erase_node(_node_base *p) {
    if (p == &m_end)
      return p;

    if (p == m_first) {
      m_first = p->next;
    } else {
      link(*p->prev, *p->next);
    }

    auto res = p->next;
    aria::destroy_at(cast(p));
    m_alloc.deallocate(cast(p), 1);
    m_size--;
    return res;
  }

  void adjust_on_empty() noexcept {
    if (m_size == 0) {
      m_end.prev = nullptr;
      m_first = &m_end;
    }
  }

  _node_base m_end;
  _node_base *m_first = &m_end;
  size_type m_size = 0;
  node_allocator_type m_alloc;
};