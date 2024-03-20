#pragma once

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

	auto operator<=>(const pair&) const = default;

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


} // namespace aria