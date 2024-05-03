#pragma once

#include "utility.h"

namespace aria {

template <class... Ts> struct type_list;

template <class T, class... Ts> struct type_list<T, Ts...> {
  using head = T;
  using tail = type_list<Ts...>;
};

template <class... Ts> struct tuple_size<type_list<Ts...>> : integral_constant<size_t, sizeof...(Ts)> {};

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

template <class U, class Tlist, template <class, class> class Trait = is_same> consteval bool exact_one_math() {
  return num_match<U, Tlist, Trait>() == 1;
}

// ============= overload match detector. to be used for variant generic assignment

template <size_t N, class A> struct overload_frag {
  using type = A;
  using arr1 = A[1];
  template <class T>
    requires requires { arr1{std::declval<T>()}; }
  auto operator()(A, T &&) -> overload_frag<N, A>;
};

template <class Seq, class... Args> struct make_overload;

template <size_t... Idx, class... Args> struct make_overload<index_sequence<Idx...>, Args...> : overload_frag<Idx, Args>... {
  using overload_frag<Idx, Args>::operator()...;
};

template <class T, class... Ts>
using best_overload_match =
    typename decltype(make_overload<make_index_sequence<sizeof...(Ts)>, Ts...>{}(std::declval<T>(), std::declval<T>()))::type;

template <class T, class... Ts>
concept has_non_ambiguous_match = requires { typename best_overload_match<T, Ts...>; };

} // namespace aria