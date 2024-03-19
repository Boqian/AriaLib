#pragma once

#include "type_traits.h"

namespace aria {

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

} // namespace aria