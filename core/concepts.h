#pragma once
#include "type_traits.h"
// https://en.cppreference.com/w/cpp/header/concepts
namespace aria {
template <class T, class U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <class From, class To>
concept convertible_to = is_convertible_v<From, To> && requires { static_cast<To>(declval<From>()); };

template <class Derived, class Base>
concept derived_from = is_base_of_v<Base, Derived> && is_constructible_v<const volatile Derived *, const volatile Base *>;

template <class T>
concept destructible = is_nothrow_destructible_v<T>;

template <class T, class... Args>
concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;

template <class T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

template <class T>
concept copy_constructible =
    move_constructible<T> && constructible_from<T, T &> && convertible_to<T &, T> && constructible_from<T, const T &> &&
    convertible_to<const T &, T> && constructible_from<T, const T> && convertible_to<const T, T>;

template <class LHS, class RHS>
concept assignable_from = is_lvalue_reference_v<LHS> && requires(LHS lhs, RHS &&rhs) {
  { lhs = forward<RHS>(rhs) } -> same_as<LHS>;
};

template <class T>
concept default_initializable = constructible_from<T> && requires { T{}; } && requires { ::new T; };

template <class T>
concept integral = is_integral_v<T>;

template <class T>
concept signed_integral = integral<T> && is_signed_v<T>;

template <class T>
concept unsigned_integral = integral<T> && is_unsigned_v<T>;

template <class T>
concept floating_point = is_floating_point_v<T>;

template <class T>
concept arithmetic = is_arithmetic_v<T>;

template <class T>
concept _boolean_testable_impl = convertible_to<T, bool>;

template <class T>
concept boolean_testable = _boolean_testable_impl<T> && requires(T &&t) {
  { !static_cast<T &&>(t) } -> _boolean_testable_impl;
};

template <class T>
concept not_void = !same_as<T, void>;

template <class T>
concept has_class_or_enumTpe = is_class_v<remove_cvref_t<T>> || is_enum_v<remove_cvref_t<T>> || is_union_v<remove_cvref_t<T>>;

namespace ranges {
namespace _Swap {
template <class T> void swap(T &, T &) = delete;

template <class T, class U>
concept _Use_ADL_swap =
    (has_class_or_enumTpe<T> || has_class_or_enumTpe<U>)&&requires(T &&t, U &&u) { swap(forward<T>(t), forward<U>(u)); };

struct _Cpo {
  template <class T, class U>
    requires _Use_ADL_swap<T, U>
  constexpr void operator()(T &&t, U &&u) const noexcept {
    swap(static_cast<T &&>(t), static_cast<U &&>(u));
  }

  template <class T>
    requires(!_Use_ADL_swap<T &, T &> && move_constructible<T> && assignable_from<T &, T>)
  constexpr void operator()(T &x, T &y) const {
    T temp = move(x);
    x = move(y);
    y = move(temp);
  }

  template <class T, class U, size_t Size>
  constexpr void operator()(T (&t)[Size], U (&u)[Size]) const noexcept(noexcept((*this)(t[0], u[0])))
    requires requires { (*this)(t[0], u[0]); }
  {
    for (size_t i = 0; i < Size; ++i)
      (*this)(t[i], u[i]);
  }
};
} // namespace _Swap

inline constexpr _Swap::_Cpo swap;

} // namespace ranges

template <class T>
concept swappable = requires(T &a, T &b) { ranges::swap(a, b); };

template <class T>
concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T &, T> && swappable<T>;

template <class T>
concept copyable =
    copy_constructible<T> && movable<T> && assignable_from<T &, T &> && assignable_from<T &, const T &> && assignable_from<T &, const T>;

template <class T>
concept semiregular = copyable<T> && default_initializable<T>;

template <class T, class U>
concept weakly_equality_comparable_with = requires(const remove_reference_t<T> &t, const remove_reference_t<U> &u) {
  { t == u } -> boolean_testable;
  { t != u } -> boolean_testable;
  { u == t } -> boolean_testable;
  { u != t } -> boolean_testable;
};

template <class T>
concept equality_comparable = weakly_equality_comparable_with<T, T>;

template <class T>
concept regular = semiregular<T> && equality_comparable<T>;

} // namespace aria