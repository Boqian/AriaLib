#pragma once

#include "algorithm.h"
#include "functional.h"
#include "iterator.h"
#include "vector.h"

namespace aria {

struct sorted_unique_t {
  explicit sorted_unique_t() = default;
};
inline constexpr sorted_unique_t sorted_unique{};

template <bool IsMulti, class Key, class Compare, class KeyContainer> class flat_set_base : public iterable_mixin {
public:
  using container_type = KeyContainer;
  using key_type = Key;
  using value_type = Key;
  using key_compare = Compare;
  using value_compare = Compare;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = typename KeyContainer::size_type;
  using difference_type = typename KeyContainer::difference_type;
  using iterator = KeyContainer::iterator;
  using const_iterator = KeyContainer::const_iterator;
  using reverse_iterator = KeyContainer::reverse_iterator;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  flat_set_base() = default;
  explicit flat_set_base(const key_compare &comp) : m_cmp(comp) {}

  explicit flat_set_base(container_type cont, const key_compare &comp = key_compare()) : m_cmp(comp), m_cont(move(cont)) { sort(); }
  explicit flat_set_base(sorted_unique_t, container_type cont, const key_compare &comp = key_compare()) : m_cmp(comp), m_cont(move(cont)) {}

  template <input_iterator It>
  flat_set_base(It first, It last, const key_compare &comp = key_compare()) : m_cont(first, last), m_cmp(comp) {
    sort();
  }

  template <input_iterator It>
  flat_set_base(sorted_unique_t, It first, It last, const key_compare &comp = key_compare()) : m_cont(first, last), m_cmp(comp) {}

  flat_set_base(initializer_list<value_type> init, const key_compare &comp = key_compare())
      : flat_set_base(init.begin(), init.end(), comp) {}

  //--------------------Capacity and iterators--------------------

  template <class S> auto begin(this S &s) noexcept { return s.m_cont.begin(); }
  template <class S> auto end(this S &s) noexcept { return s.m_cont.end(); }
  size_type size() const noexcept { return m_cont.size(); }
  bool empty() const noexcept { return size() == 0; }

  //--------------------Lookup--------------------

  const_iterator find(const Key &key) const {
    auto pos = lower_bound(key);
    return (pos == end() || m_cmp(key, *pos)) ? end() : pos;
  }

  iterator find(const Key &key) { return as_const(*this).find(key); }
  bool contains(const Key &key) const { return find(key) != end(); }

  const_iterator lower_bound(const Key &key) const { return aria::lower_bound(begin(), end(), key, m_cmp); }
  iterator lower_bound(const Key &key) { return as_const(*this).lower_bound(key); }
  const_iterator upper_bound(const Key &key) const { return aria::upper_bound(begin(), end(), key, m_cmp); }
  iterator upper_bound(const Key &key) { return as_const(*this).upper_bound(key); }
  pair<const_iterator, const_iterator> equal_range(const Key &key) const { return {lower_bound(key), upper_bound(key)}; }
  pair<iterator, iterator> equal_range(const Key &key) { return {lower_bound(key), upper_bound(key)}; }

  //--------------------Modifiers--------------------

  iterator erase(iterator pos) { return m_cont.erase(pos); }

  template <class S, class... Args> auto emplace(this S &&s, Args &&...args) { return s.insert(value_type(forward<Args>(args)...)); }

  container_type extract() && { return move(m_cont); }

  void replace(container_type &&cont) { m_cont = move(cont); }

  void clear() noexcept { m_cont.clear(); }

  void swap(flat_set_base &other) noexcept { aria::swap(m_cont, other.m_cont); }

  bool operator==(const flat_set_base &rhs) const noexcept { return m_cont == rhs.m_cont; }

protected:
  void sort() { aria::sort(begin(), end()); }

  key_compare m_cmp;
  container_type m_cont;
};

template <class Key, class Compare = less<Key>, class KeyContainer = vector<Key>>
class flat_set : public flat_set_base<false, Key, Compare, KeyContainer> {
public:
  using Base = flat_set_base<false, Key, Compare, KeyContainer>;
  using key_type = Key;
  using value_type = Key;
  using pointer = value_type *;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using key_compare = Compare;
  using value_compare = Compare;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;
  using Base::Base;

  size_type count(const Key &key) const { return Base::contains(key) ? 1 : 0; }

  template <class U> requires same_as<remove_cvref_t<U>, value_type> pair<iterator, bool> insert(U &&value) {
    auto pos = Base::lower_bound(value);
    if (pos == Base::end() || Base::m_cmp(value, *pos)) {
      return {Base::m_cont.insert(pos, forward<U>(value)), true};
    } else {
      return {pos, false};
    }
  }
};

template <class Key, class Compare = less<Key>, class KeyContainer = vector<Key>>
class flat_multiset : public flat_set_base<true, Key, Compare, KeyContainer> {
public:
  using Base = flat_set_base<true, Key, Compare, KeyContainer>;
  using key_type = Key;
  using value_type = Key;
  using pointer = value_type *;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using key_compare = Compare;
  using value_compare = Compare;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;
  using Base::Base;

  size_type count(const Key &key) const { return Base::upper_bound(key) - Base::lower_bound(key); }

  template <class U> requires same_as<remove_cvref_t<U>, value_type> iterator insert(U &&value) {
    return Base::m_cont.insert(Base::upper_bound(value), forward<U>(value));
  }
};

} // namespace aria