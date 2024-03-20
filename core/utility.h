#pragma once

#include <cstddef>
#include "type_traits.h"
#include "concepts.h"
#include <compare>

/*
nullptr_t
move, forward
swap
pair
*/

namespace aria {
using std::size_t;
using nullptr_t = decltype(nullptr);

template <class T> constexpr remove_reference_t<T> &&move(T &&t) noexcept {
  return static_cast<remove_reference_t<T> &&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T> &t) noexcept {
  return static_cast<T &&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T> &&t) noexcept {
  static_assert(!is_lvalue_reference_v<T>);
  return static_cast<T &&>(t);
}

template <class T> constexpr void swap(T &a, T &b) noexcept {
  T temp = move(a);
  a = move(b);
  b = move(temp);
}

//------------------------- pair -------------------------//
template<class T1, class T2> struct pair {
public:
	using first_type = T1;
	using second_type = T2;

	constexpr pair() : first{}, second{} {}
    constexpr pair(const T1 &x, const T2 &y) : first(x), second(y) {}
    constexpr pair(const pair &) = default;
    constexpr pair(pair &&) noexcept = default;
    constexpr pair &operator=(const pair &) noexcept = default;
    constexpr pair &operator=(pair &&) noexcept = default;

	template <class U1, class U2>
    constexpr pair(U1 &&x, U2 &&y)
        : first{forward<U1>(x)}, second{forward<U2>(y)} {}

    constexpr void swap(pair &rhs) noexcept {
          ::aria::swap(first, rhs.first);
          ::aria::swap(second, rhs.second);
        }

	const auto operator<=>(const pair&) const = default;

	T1 first;
    T2 second;
};

template <class T1, class T2> pair(T1, T2) -> pair<T1, T2>;

template <class T1, class T2> 
constexpr pair<T1, T2> make_pair(const T1 &&a, const T2 &&b) {
  return pair(a, b);
}

template <class T1, class T2> void swap(pair<T1, T2> &a, pair<T1, T2> &b) {
  a.swap(b);
}

//------------------------- tuple -------------------------//
template<class... Args> class tuple {};
template <> class tuple<> {
public:
  constexpr auto operator<=>(const tuple &) const = default;
};
template <class T, class... Args>
class tuple<T, Args...>{
public:
  constexpr tuple() = default;
  constexpr tuple(const T& t, const Args&...args)
      : value(t), rest(args...){}

  constexpr auto operator<=>(const tuple &) const = default;

  T value;
  tuple<Args...> rest;
};

template <typename> struct tuple_size;
template <class... Args>
struct tuple_size<tuple<Args...>> : integral_constant<size_t, sizeof...(Args)> {};
template<class T> inline constexpr size_t tuple_size_v = tuple_size<remove_cv_t<T>>::value;

template <size_t I, class... Types>
constexpr const auto &get(const tuple<Types...> &t) noexcept {
  if constexpr (I == 0)
    return t.value;
  else
    return get<I - 1>(t.rest);
}

template <size_t I, class... Types>
constexpr auto &get(tuple<Types...> &t) noexcept {
  if constexpr (I == 0)
    return t.value;
  else
    return get<I - 1>(t.rest);
}

template <class T, class... Types>
constexpr const auto &get(const tuple<Types...> &t) noexcept {
  if constexpr (is_same_v<T, decltype(t.value)>)
    return t.value;
  else
    return get<T>(t.rest);
}

template <class T, class... Types>
constexpr auto &get(tuple<Types...> &t) noexcept {
  if constexpr (is_same_v<T, decltype(t.value)>)
    return t.value;
  else
    return get<T>(t.rest);
}

template <size_t, typename> struct tuple_element;
template <size_t I, class... Args>
struct tuple_element<I, tuple<Args...>> {
   using type = remove_cvref_t<decltype(get<I>(tuple<Args...>()))>;
};

} // namespace aria