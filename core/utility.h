#pragma once

#include "concepts.h"
#include "cstddef.h"
#include "type_traits.h"
#include <compare>
#include <cstring> //memcpy
#include <initializer_list>

/*
initializer_list
move, forward //in type_traits.h
as_const
integer_sequence
pair, make_pair
tuple, tupe_element, get(), tupe_size
unreachable()
exchange
in_place_t
addressof
pointer_traits
*/

namespace aria {

using std::initializer_list;

//----------------- as_const ------------------
template <class T> const T &as_const(T &t) noexcept { return t; }
template <class T> const T &as_const(T &&) = delete;

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
    using aria::swap;
    swap(first, rhs.first);
    swap(second, rhs.second);
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
    using aria::swap;
    swap(value, rhs.value);
    rest.swap(rhs.rest);
  }

  T value;
  tuple<Args...> rest;
};

template <class... UTypes> tuple(UTypes...) -> tuple<UTypes...>;

template <typename> struct tuple_size;
template <class... Args> struct tuple_size<tuple<Args...>> : integral_constant<size_t, sizeof...(Args)> {};
template <class T> inline constexpr size_t tuple_size_v = tuple_size<remove_cv_t<T>>::value;
template <class K, class V> struct tuple_size<pair<K, V>> : integral_constant<size_t, 2> {};

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

//------------------------- integer_sequence -------------------------//
template <class T, T... Ints> struct integer_sequence {
  static constexpr size_t size() noexcept { return sizeof...(Ints); }
};

template <size_t... Ints> using index_sequence = integer_sequence<size_t, Ints...>;

template <class T, T a, T... Ints> constexpr auto _append_integer_sequence(integer_sequence<T, Ints...> seq) {
  return integer_sequence<T, Ints..., a>();
}

template <class T, T N> constexpr auto _integer_sequence_maker() {
  if constexpr (N == 0) {
    return integer_sequence<T>();
  } else {
    return _append_integer_sequence<T, N - 1>(_integer_sequence_maker<T, N - 1>());
  }
}

template <class T, T N> using make_integer_sequence = decltype(_integer_sequence_maker<T, N>());

template <size_t N> using make_index_sequence = make_integer_sequence<size_t, N>;

//------------------------- unreachable() -------------------------//
[[noreturn]] inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
  __assume(false);
#else // GCC, Clang
  __builtin_unreachable();
#endif
}

//------------------------- exchange() -------------------------//
template <class T, class U = T> constexpr T exchange(T &obj, U &&new_value) {
  T old_value = move(obj);
  obj = forward<U>(new_value);
  return old_value;
}

//------------------------- in_place -------------------------//
struct in_place_t {
  explicit in_place_t() = default;
};
inline constexpr in_place_t in_place{};

//------------------------- addressof -------------------------//
template <class T> auto addressof(T &x) noexcept {
  if constexpr (is_object_v<T>)
    return reinterpret_cast<T *>(&const_cast<char &>(reinterpret_cast<const volatile char &>(x)));
  else
    return &x;
}

//------------------------- pointer_traits -------------------------//
template <class T> struct _get_ptr_difference_type : type_identity<ptrdiff_t> {};
template <class T>
  requires requires { typename T::difference_type; }
struct _get_ptr_difference_type<T> : type_identity<typename T::difference_type> {};

template <class Ptr> struct pointer_traits {
  using pointer = Ptr;
  using difference_type = typename _get_ptr_difference_type<Ptr>::type;
  // todo using element_type = ...
};

template <class T> struct pointer_traits<T *> {
  using pointer = T *;
  using element_type = T;
  using difference_type = ptrdiff_t;

  template <class U> using rebind = U *;
  static pointer pointer_to(element_type &val) noexcept { return std::addressof(val); }
};

template <class T>
concept _has_to_address = requires(const T &val) {
  typename pointer_traits<T>;
  pointer_traits<T>::to_address(val);
};

template <class T> constexpr T *to_address(T *const val) noexcept {
  static_assert(!is_function_v<T>);
  return val;
}

template <class Ptr> constexpr auto to_address(const Ptr &val) noexcept {
  if constexpr (_has_to_address<Ptr>) {
    return pointer_traits<Ptr>::to_address(val);
  } else {
    return to_address(val.operator->());
  }
}

} // namespace aria