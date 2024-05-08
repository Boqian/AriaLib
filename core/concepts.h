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
concept integral = is_integral_v<T>;

template <class T>
concept signed_integral = integral<T> && is_signed_v<T>;

template <class T>
concept unsigned_integral = integral<T> && is_unsigned_v<T>;

} // namespace aria