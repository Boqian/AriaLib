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
concept not_void = !same_as<T, void>;

// namespace ranges {
// namespace _Swap {
// template <class T> void swap(T &, T &) = delete;
//
// template <class _Ty1, class _Ty2>
// concept _Use_ADL_swap = (_Has_class_or_enum_type<_Ty1> || _Has_class_or_enum_type<_Ty2>)&&requires(_Ty1 &&__t, _Ty2 &&__u) {
//   swap(static_cast<_Ty1 &&>(__t), static_cast<_Ty2 &&>(__u)); // intentional ADL
// };
//
// struct _Cpo {
//   template <class _Ty1, class _Ty2>
//     requires _Use_ADL_swap<_Ty1, _Ty2>
//   constexpr void operator()(_Ty1 &&__t, _Ty2 &&__u) const
//       noexcept(noexcept(swap(static_cast<_Ty1 &&>(__t), static_cast<_Ty2 &&>(__u)))) { // intentional ADL
//     swap(static_cast<_Ty1 &&>(__t), static_cast<_Ty2 &&>(__u));                        // intentional ADL
//   }
//
//   template <class _Ty>
//     requires(!_Use_ADL_swap<_Ty &, _Ty &> && move_constructible<_Ty> && assignable_from<_Ty &, _Ty>)
//   constexpr void operator()(_Ty &__x, _Ty &__y) const noexcept(is_nothrow_move_constructible_v<_Ty> && is_nothrow_move_assignable_v<_Ty>)
//   {
//     _Ty __tmp(static_cast<_Ty &&>(__x));
//     __x = static_cast<_Ty &&>(__y);
//     __y = static_cast<_Ty &&>(__tmp);
//   }
//
//   template <class _Ty1, class _Ty2, size_t _Size>
//   constexpr void operator()(_Ty1 (&__t)[_Size], _Ty2 (&__u)[_Size]) const noexcept(noexcept((*this)(__t[0], __u[0])))
//     requires requires { (*this)(__t[0], __u[0]); }
//   {
//     for (size_t __i = 0; __i < _Size; ++__i) {
//       (*this)(__t[__i], __u[__i]);
//     }
//   }
// };
// } // namespace _Swap
//
// inline namespace _Cpos {
//_EXPORT_STD inline constexpr _Swap::_Cpo swap;
// }
// } // namespace ranges

} // namespace aria