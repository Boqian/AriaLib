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

//----------------- conditional -----------------------
template<bool B, class T, class F>
struct conditional { using type = T; };
 
template<class T, class F>
struct conditional<false, T, F> { using type = F; };

template <bool B, class T, class F>
using conditional_t = conditional<B, T, F>::type;

//----------------- is_same -----------------------
template <class T, class U>
struct is_same : false_type {};

template <class T>
struct is_same<T, T> : true_type {};

template<class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

//----------------- is_convertible -----------------------
namespace details {
template <class To>
inline void foo(To) {}

template <typename From, typename To>
concept convertible = requires(From f) { foo<To>(f); };

}  // namespace detail

template <class From, class To>
struct is_convertible : conditional_t<details::convertible<From, To>, true_type, false_type> {};

template<class From, class To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

}  // namespace aria