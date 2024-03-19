#pragma once

#include "type_traits.h"

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

} // namespace aria