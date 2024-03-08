#pragma once

/*
* https://en.cppreference.com/w/cpp/header/type_traits
*/

namespace aria {

template <class T, T v>
struct integral_constant {
  using value_type = T;
  static constexpr T value = v;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

//----------------- is_same -----------------------
template <class T, class U>
struct is_same : false_type {};

template <class T>
struct is_same<T, T> : true_type {};

template<class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

}  // namespace aria