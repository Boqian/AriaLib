#pragma once

#include "concepts.h"
#include "type_traits.h"
#include <compare>
#include <cstddef>
#include <cstring> //memcpy
#include <initializer_list>
#include <stdexcept>

/*
initializer_list
size_t, ptrdiff_t, nullptr_t
move, forward //in type_traits.h
as_const
swap
pair, make_pair
tuple, tupe_element, get(), tupe_size
*/

namespace aria {
using std::ptrdiff_t;
using std::size_t;
using nullptr_t = decltype(nullptr);
using std::initializer_list;

//----------------- move, forward------------------
template <class T> constexpr remove_reference_t<T> &&move(T &&t) noexcept { return static_cast<remove_reference_t<T> &&>(t); }

template <typename T> constexpr T &&forward(remove_reference_t<T> &t) noexcept { return static_cast<T &&>(t); }

template <typename T> constexpr T &&forward(remove_reference_t<T> &&t) noexcept {
  static_assert(!is_lvalue_reference_v<T>);
  return static_cast<T &&>(t);
}

//----------------- as_const ------------------
template <class T> const T &as_const(T &t) noexcept { return t; }
template <class T> const T &as_const(T &&) = delete;

//----------------- swap ------------------
template <class T> constexpr void swap(T &a, T &b) noexcept {
  T temp = move(a);
  a = move(b);
  b = move(temp);
}

//------------------------- pair -------------------------//
template <class T1, class T2> struct pair {
public:
  using first_type = T1;
  using second_type = T2;

  constexpr pair() : first{}, second{} {}
  constexpr pair(const T1 &x, const T2 &y) : first(x), second(y) {}
  constexpr pair(const pair &) = default;
  constexpr pair(pair &&) noexcept = default;
  constexpr pair &operator=(const pair &) noexcept = default;
  constexpr pair &operator=(pair &&) noexcept = default;

  template <class U1, class U2> constexpr pair(U1 &&x, U2 &&y) : first{forward<U1>(x)}, second{forward<U2>(y)} {}
  template <class U1, class U2> constexpr pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}

  constexpr void swap(pair &rhs) noexcept {
    ::aria::swap(first, rhs.first);
    ::aria::swap(second, rhs.second);
  }

  const auto operator<=>(const pair &) const = default;

  T1 first;
  T2 second;
};

template <class T1, class T2> pair(T1, T2) -> pair<T1, T2>;

template <class T1, class T2> constexpr pair<T1, T2> make_pair(const T1 &&a, const T2 &&b) { return pair(a, b); }

template <class T1, class T2> void swap(pair<T1, T2> &a, pair<T1, T2> &b) { a.swap(b); }

template <class T> struct is_pair : false_type {};
template <class T1, class T2> struct is_pair<pair<T1, T2>> : true_type {};
template <class T> inline constexpr bool is_pair_v = is_pair<T>::value;

//------------------------- tuple -------------------------//
template <class... Args> class tuple {};
template <> class tuple<> {
public:
  constexpr tuple() = default;
  constexpr auto operator<=>(const tuple &) const = default;
  void swap(tuple &rhs) noexcept {}
};

template <class T, class... Args> class tuple<T, Args...> {
public:
  constexpr tuple() = default;
  constexpr tuple(const T &t, const Args &...args) : value(t), rest(args...) {}
  constexpr tuple(const tuple &) = default;
  constexpr tuple(tuple &&) noexcept = default;
  constexpr tuple &operator=(const tuple &) noexcept = default;
  constexpr tuple &operator=(tuple &&) noexcept = default;

  template <class U, class... Us>
    requires convertible_to<U, T> && convertible_to<Us..., Args...>
  constexpr tuple(U &&u, Us &&...args) : value(forward<U>(u)), rest(forward<Us>(args)...) {}

  constexpr auto operator<=>(const tuple &) const = default;

  void swap(tuple &rhs) noexcept {
    ::aria::swap(value, rhs.value);
    rest.swap(rhs.rest);
  }

  T value;
  tuple<Args...> rest;
};

template <class... UTypes> tuple(UTypes...) -> tuple<UTypes...>;

template <typename> struct tuple_size;
template <class... Args> struct tuple_size<tuple<Args...>> : integral_constant<size_t, sizeof...(Args)> {};
template <class T> inline constexpr size_t tuple_size_v = tuple_size<remove_cv_t<T>>::value;

template <size_t I, class... Types> constexpr const auto &get(const tuple<Types...> &t) noexcept {
  if constexpr (I == 0)
    return t.value;
  else
    return get<I - 1>(t.rest);
}

template <size_t I, class... Types> constexpr auto &get(tuple<Types...> &t) noexcept {
  if constexpr (I == 0)
    return t.value;
  else
    return get<I - 1>(t.rest);
}

template <class T, class... Types> constexpr const auto &get(const tuple<Types...> &t) noexcept {
  if constexpr (is_same_v<T, decltype(t.value)>)
    return t.value;
  else
    return get<T>(t.rest);
}

template <class T, class... Types> constexpr auto &get(tuple<Types...> &t) noexcept {
  if constexpr (is_same_v<T, decltype(t.value)>)
    return t.value;
  else
    return get<T>(t.rest);
}

template <size_t, typename> struct tuple_element;
template <size_t I, class... Args> struct tuple_element<I, tuple<Args...>> {
  using type = remove_cvref_t<decltype(get<I>(tuple<Args...>()))>;
};

template <class... Args> tuple<Args...> make_tuple(Args &&...args) { return tuple<Args...>(forward<Args>(args)...); }

template <class... Args> void swap(tuple<Args...> &a, tuple<Args...> &b) { a.swap(b); }

} // namespace aria