#pragma once

#include "flat_set.h"

namespace aria {

// template <class FlatMapType> class flat_map_iterator {
// public:
//   using value_type = FlatMapType::value_type;
//   using key_type = FlatMapType::key_type;
//   using mapped_type = FlatMapType::mapped_type;
//
// private:
//   typename FlatMapType::key_container_type &m_keys;
//   typename FlatMapType::mapped_container_type &m_values;
//   vector<size_t> &m_index;
//   size_t m_idx;
// };

template <class Key, class T, class Compare, class KeyContainer, class MappedContainer> class flat_map_base {
public:
  using key_container_type = KeyContainer;
  using mapped_container_type = MappedContainer;
  using key_type = Key;
  using mapped_type = T;
  using value_type = pair<key_type, mapped_type>;
  using key_compare = Compare;
  using reference = pair<const key_type &, mapped_type &>;
  using const_reference = pair<const key_type &, const mapped_type &>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  // using iterator = KeyContainer::iterator;
  // using const_iterator = KeyContainer::const_iterator;
  // using reverse_iterator = KeyContainer::reverse_iterator;
  // using const_reverse_iterator = aria::reverse_iterator<const_iterator>;

  explicit flat_map_base(const key_compare &comp) : m_cmp(comp), m_c() {}
  flat_map_base() : m_cmp(key_compare()), m_c() {}

  flat_map_base(key_container_type key_cont, mapped_container_type mapped_cont, const key_compare &comp = key_compare()) {}

  // explicit flat_map_base(container_type cont, const key_compare &comp = key_compare()) : m_cmp(comp), m_cont(move(cont)) { sort(); }
  // explicit flat_map_base(sorted_unique_t, container_type cont, const key_compare &comp = key_compare()) : m_cmp(comp), m_cont(move(cont))
  // {}

  // template <input_iterator It>
  // flat_map_base(It first, It last, const key_compare &comp = key_compare()) : m_cont(first, last), m_cmp(comp) {
  //   sort();
  // }

  // template <input_iterator It>
  // flat_map_base(sorted_unique_t, It first, It last, const key_compare &comp = key_compare()) : m_cont(first, last), m_cmp(comp) {}

  // flat_map_base(initializer_list<value_type> init, const key_compare &comp = key_compare())
  //     : flat_set_base(init.begin(), init.end(), comp) {}

  const key_container_type &keys() const noexcept { return m_c.keys; }
  const mapped_container_type &values() const noexcept { return m_c.values; }
  size_type size() const noexcept { return keys().size(); }
  bool empty() const noexcept { return size() == 0; }

  struct containers {
    containers() = default;
    template <class KeyCont, class MappedCont>
    containers(KeyCont &&key_cont, MappedCont &&mapped_cont) : keys(forward<KeyCont>(key_cont)), values(forward<MappedCont>(mapped_cont)) {}
    KeyContainer keys;
    MappedContainer values;
  };

protected:
  key_compare m_cmp;
  containers m_c;
};

template <class Key, class T, class Compare = less<Key>, class KeyContainer = vector<Key>, class MappedContainer = vector<T>>
class flat_map : public flat_map_base<Key, T, Compare, KeyContainer, MappedContainer> {
public:
  using Base = flat_map_base<Key, T, Compare, KeyContainer, MappedContainer>;
  using key_container_type = KeyContainer;
  using mapped_container_type = MappedContainer;
  using key_type = Key;
  using mapped_type = T;
  using value_type = Base::value_type;
  using key_compare = Compare;
  using value_compare = Compare;
  using reference = Base::reference;
  using const_reference = Base::const_reference;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using containers = Base::containers;
  using Base::Base;

  // size_type count(const Key &key) const { return Base::contains(key) ? 1 : 0; }

  // template <class U> requires same_as<remove_cvref_t<U>, value_type> pair<iterator, bool> insert(U &&value) {
  //   auto pos = Base::lower_bound(value);
  //   if (pos == Base::end() || Base::m_cmp(value, *pos)) {
  //     return {Base::m_cont.insert(pos, forward<U>(value)), true};
  //   } else {
  //     return {pos, false};
  //   }
  // }
};

} // namespace aria