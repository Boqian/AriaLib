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

  template <class S> auto begin(this S &s) noexcept { return s.m_cont.begin(); }
  template <class S> auto end(this S &s) noexcept { return s.m_cont.end(); }
  size_type size() const noexcept { return m_cont.size(); }
  bool empty() const noexcept { return size() == 0; }

  const_iterator find(const Key &key) const {
    auto pos = lower_bound(begin(), end(), key, m_cmp);
    return (pos == end() || m_cmp(key, *pos)) ? end() : pos;
  }

  iterator find(const Key &key) { return as_const(*this).find(key); }
  bool contains(const Key &key) const { return find(key) != end(); }

  template <class U> requires same_as<remove_cvref_t<U>, value_type> pair<iterator, bool> insert(U &&value) {
    auto pos = find_insert_position(value);
    if constexpr (IsMulti) {
      return {m_cont.insert(pos, forward<U>(value)), true};
    } else {
      if (pos == end() || m_cmp(value, *pos)) {
        return {m_cont.insert(pos, forward<U>(value)), true};
      } else
        return {pos, false};
    }
  }

  bool operator==(const flat_set_base &rhs) const noexcept { return m_cont == rhs.m_cont; }

protected:
  void sort() { aria::sort(begin(), end()); }

  iterator find_insert_position(const key_type &key) const {
    if constexpr (IsMulti)
      return upper_bound(begin(), end(), key, m_cmp);
    else
      return lower_bound(begin(), end(), key, m_cmp);
  }

  key_compare m_cmp;
  container_type m_cont;
};

template <class Key, class Compare = less<Key>, class KeyContainer = vector<Key>>
class flat_set : public flat_set_base<false, Key, Compare, KeyContainer> {
public:
  using base = flat_set_base<false, Key, Compare, KeyContainer>;
  using base::base;
};
} // namespace aria