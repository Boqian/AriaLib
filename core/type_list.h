#pragma once

#include "utility.h"

namespace aria {

template <class... Ts> struct type_list;

template <class T, class... Ts> struct type_list<T, Ts...> {
  using head = T;
  using tail = type_list<Ts...>;
};

template <class... Ts> struct tuple_size<type_list<Ts...>> : integral_constant<size_t, sizeof...(Ts)> {};

template <size_t I, class Tlist> consteval auto type_list_type() {
  static_assert(tuple_size_v<Tlist> > 0);
  if constexpr (I == 0) {
    return declval<typename Tlist::head>();
  } else {
    return type_list_type<I - 1, typename Tlist::tail>();
  }
}

template <size_t I, class Tlist> using nth_of_type_list = decltype(type_list_type<I, Tlist>());
template <size_t I, class... Ts> using nth_type = decltype(type_list_type<I, type_list<Ts...>>());

inline constexpr size_t type_list_npos = -1;

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval size_t first_match_index() {
  if constexpr (tuple_size_v<Tlist> == 0) {
    return -1;
  } else if constexpr (Trait<typename Tlist::head, U>::value) {
    return 0;
  } else {
    return 1 + first_match_index<U, typename Tlist::tail, Trait>();
  }
}

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval size_t num_match() {
  if constexpr (tuple_size_v<Tlist> == 0) {
    return 0;
  } else {
    return Trait<typename Tlist::head, U>::value + num_match<U, typename Tlist::tail, Trait>();
  }
}

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval bool exact_one_match() {
  return num_match<U, Tlist, Trait>() == 1;
}

template <class T, class... Ts>
  requires(exact_one_match<T, type_list<Ts...>>())
consteval size_t index_of() {
  return first_match_index<T, type_list<Ts...>>();
}

} // namespace aria