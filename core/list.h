#pragma once
#include "algorithm.h"
#include "allocator.h"
#include "iterator.h"
#include "utility.h"

namespace aria {
namespace _list {
struct node_base {
  node_base *prev = nullptr;
  node_base *next = nullptr;
};

template <class T> struct node : public node_base {
  template <class... Args> requires is_constructible_v<T, Args...> node(Args &&...args) : value(forward<Args>(args)...) {}
  T value;
};
} // namespace _list

template <class ListType> class list_const_iterator {
public:
  using iterator_concept = bidirectional_iterator_tag;
  using value_type = typename ListType::value_type;
  using pointer = typename ListType::const_pointer;
  using reference = const value_type &;
  using difference_type = ptrdiff_t;
  using node_type = _list::node<value_type>;
  using node_base_type = _list::node_base;
  friend ListType;

  list_const_iterator() : ptr(nullptr) {}
  explicit list_const_iterator(const node_base_type *p) : ptr(p) {}

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
  const node_base_type *ptr;
};

template <class T, class Allocator = allocator<T>> class list : public iterable {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using allocator_type = Allocator;
  using pointer = typename allocator_traits<Allocator>::pointer;
  using const_pointer = typename allocator_traits<Allocator>::const_pointer;
  using const_iterator = list_const_iterator<list<T, Allocator>>;
  using iterator = mutable_iterator<const_iterator>;
  using reverse_iterator = reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<iterator>;

  list() noexcept = default;
  ~list() noexcept { clear(); }

  list(initializer_list<value_type> init) {
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
    auto temp = move(rhs);
    swap(temp);
    return *this;
  }

  template <class... Args> reference emplace_back(Args &&...args) { return *iterator(insert_value(m_end, forward<Args>(args)...)); }
  template <class... Args> reference emplace_front(Args &&...args) { return *iterator(insert_value(m_first, forward<Args>(args)...)); }
  void push_back(value_type value) { insert_value(m_end, move(value)); }
  void push_front(value_type value) { insert_value(m_first, move(value)); }
  void pop_back() noexcept { erase_node(last()); }
  void pop_front() noexcept { erase_node(m_first); }
  reference front() noexcept { return first()->value; }
  const_reference front() const noexcept { return first()->value; }
  reference back() noexcept { return last()->value; }
  const_reference back() const noexcept { return last()->value; }
  bool empty() const noexcept { return m_size == 0; }
  size_type size() const noexcept { return m_size; }
  allocator_type get_allocator() const { return m_alloc; }

  void clear() noexcept {
    while (!empty())
      pop_back();
  }

  auto begin() const noexcept { return const_iterator(m_first); }
  auto end() const noexcept { return const_iterator(m_end); }
  auto begin() noexcept { return iterator(m_first); }
  auto end() noexcept { return iterator(m_end); }

  iterator insert(const_iterator pos, value_type value) { return iterator(insert_value(get_ptr(pos), move(value))); }
  iterator erase(const_iterator pos) { return iterator(erase_node(get_ptr(pos))); }

  void swap(list &rhs) noexcept {
    auto lhs_last = m_end->prev;
    auto rhs_last = rhs.m_end->prev;
    if (lhs_last)
      link(lhs_last, rhs.m_end);
    if (rhs_last)
      link(rhs_last, m_end);

    using aria::swap;
    swap(m_first, rhs.m_first);
    swap(m_size, rhs.m_size);
    swap(m_alloc, rhs.m_alloc);

    adjust_on_empty();
    rhs.adjust_on_empty();
  }

  void reverse() noexcept {
    if (size() < 2)
      return;

    auto last = m_end->prev, first = m_first;
    for (auto p = first, q = first->next; p != last;) {
      auto qq = q->next;
      link(q, p);
      p = q;
      q = qq;
    }
    link(first, m_end);
    m_first = last;
  }

  template <class Compare> void merge(list &rhs, Compare cmp) {
    m_first = merge(m_first, m_end, rhs.m_first, rhs.m_end, cmp);
    rhs.m_size = 0;
    rhs.adjust_on_empty();
  }

  void merge(list &rhs) { merge(rhs, less()); }

  template <class Compare> void sort(Compare comp) { m_first = sort(m_first, m_end, size(), comp); }
  void sort() { sort(less()); }

  template <class BinaryPredicate> size_type unique(BinaryPredicate pred) {
    if (size() <= 1)
      return 0;

    size_type res = 0;
    for (auto p = begin(), q = next(p); q != end(); p = q, q = next(q)) {
      if (pred(*p, *q)) {
        erase(p);
        res++;
      }
    }
    return res;
  }

  size_type unique() { return unique(equal_to()); }

  template <class UnaryPredicate> size_type remove_if(UnaryPredicate pred) {
    size_type res = 0;
    for (auto it = begin(); it != end();) {
      if (pred(*it)) {
        erase(it++);
        res++;
      } else
        it++;
    }
    return res;
  }

  size_type remove(const T &value) {
    return remove_if([&](auto &x) { return x == value; });
  }

  void splice(const_iterator pos, list &other, const_iterator it) {
    if (&other == this && pos == it)
      return;

    auto p = other.extract_node(const_cast<node_base_type *>(it.ptr));
    insert_node(const_cast<node_base_type *>(pos.ptr), p);
  }

private:
  using node_type = _list::node<value_type>;
  using node_base_type = _list::node_base;
  // using node_allocator_type = typename allocator_traits<Allocator>::template rebind_alloc<node_type>;
  using node_allocator_type = typename Allocator::template rebind_alloc<node_type>;

  node_type *cast(node_base_type *p) const noexcept { return static_cast<node_type *>(p); }
  node_type *last() const noexcept { return cast(m_end->prev); }
  node_type *first() const noexcept { return cast(m_first); }
  static node_base_type *get_ptr(const_iterator pos) noexcept { return const_cast<node_base_type *>(pos.ptr); }

  static void link(node_base_type *first, node_base_type *second) noexcept {
    first->next = second;
    second->prev = first;
  }

  template <class... Args> auto create_node(Args &&...args) {
    auto p = m_alloc.allocate(1);
    construct_at(p, forward<Args>(args)...);
    return p;
  }

  node_base_type *insert_node(node_base_type *pos, node_base_type *p) {
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

  node_base_type *extract_node(node_base_type *p) {
    if (p == m_end || !p)
      return p;

    if (p == m_first) {
      m_first = p->next;
      p->next->prev = nullptr;
    } else {
      link(p->prev, p->next);
    }

    m_size--;
    adjust_on_empty();

    p->next = p->prev = nullptr;
    return p;
  }

  template <class... Args> node_base_type *insert_value(node_base_type *pos, Args &&...args) {
    return insert_node(pos, create_node(forward<Args>(args)...));
  }

  node_base_type *erase_node(node_base_type *p) {
    if (p == m_end)
      return p;

    if (p == m_first) {
      m_first = p->next;
      p->next->prev = nullptr;
    } else {
      link(p->prev, p->next);
    }

    auto res = p->next;
    destroy_at(cast(p));
    m_alloc.deallocate(cast(p), 1);
    m_size--;
    return res;
  }

  void adjust_on_empty() noexcept {
    if (m_size == 0) {
      m_end->prev = nullptr;
      m_first = m_end;
    }
  }

  // return the head.  the end of merged list is end1
  template <class Compare>
  node_base_type *merge(node_base_type *p1, node_base_type *end1, node_base_type *p2, node_base_type *end2, const Compare &cmp) {
    node_base_type fake_head{};
    link(&fake_head, p1);
    while ((p1 != end1) && (p2 != end2)) {
      if (cmp(static_cast<node_type *>(p1)->value, static_cast<node_type *>(p2)->value)) {
        p1 = p1->next;
      } else {
        auto next_p2 = p2->next;
        link(p1->prev, p2);
        link(p2, p1);
        p2 = next_p2;
      }
    }
    if (p2 != end2) {
      link(p1->prev, p2);
      link(end2->prev, end1);
    }
    return fake_head.next;
  }

  node_base_type *advance(node_base_type *p, size_t n) {
    while (n--)
      p = p->next;
    return p;
  }

  template <class Compare> node_base_type *sort(node_base_type *start, node_base_type *end, size_type n, Compare cmp) {
    if (n <= 1)
      return start;

    node_base_type fake_end1{};
    auto mid = advance(start, n / 2);
    auto start1 = sort(start, mid, n / 2, cmp);
    link(mid->prev, &fake_end1);
    auto start2 = sort(mid, end, n - (n / 2), cmp);
    return merge(start2, end, start1, &fake_end1, cmp);
  }

  node_base_type m_end_node;
  node_base_type *const m_end = &m_end_node;
  node_base_type *m_first = m_end;
  size_type m_size = 0;
  node_allocator_type m_alloc;
};

template <class T, class Alloc> constexpr bool operator==(const list<T, Alloc> &a, const list<T, Alloc> &b) noexcept {
  return (&a == &b) || equal(begin(a), end(a), begin(b), end(b));
}

template <class T, class Alloc> constexpr void swap(list<T, Alloc> &a, list<T, Alloc> &b) noexcept { a.swap(b); }

template <class T, class Alloc, class U> list<T, Alloc>::size_type erase(list<T, Alloc> &a, const U &value) {
  return a.remove_if([&](auto &elem) { return elem == value; });
}

template <class T, class Alloc, class Pred> list<T, Alloc>::size_type erase_if(list<T, Alloc> &a, Pred pred) { return a.remove_if(pred); }

} // namespace aria