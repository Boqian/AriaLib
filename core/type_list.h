#pragma once

#include "utility.h"

namespace aria {

template <class... Ts> struct type_list;

template <class T, class... Ts> struct type_list<T, Ts...> {
  using head = T;
  using rest = type_list<Ts...>;
};

template <class... Ts> struct tuple_size<type_list<Ts...>> : integral_constant<size_t, sizeof...(Ts)> {};

inline constexpr size_t type_list_npos = -1;

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval size_t first_match_index() {
  if constexpr (tuple_size_v<Tlist> == 0) {
    return -1;
  } else if constexpr (Trait<typename Tlist::head, U>::value) {
    return 0;
  } else {
    return 1 + first_match_index<U, typename Tlist::rest, Trait>();
  }
}

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval size_t num_match() {
  if constexpr (tuple_size_v<Tlist> == 0) {
    return 0;
  } else {
    return Trait<typename Tlist::head, U>::value + num_match<U, typename Tlist::rest, Trait>();
  }
}

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval bool exact_one_math() {
  return num_match<U, Tlist, Trait>() == 1;
}

} // namespace aria