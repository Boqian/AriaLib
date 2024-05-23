#pragma once
#include "cstddef.h"
#include <type_traits> //for complier implemented traits
// https://en.cppreference.com/w/cpp/header/type_traits

/*
 * integral_constant
 * move, forward
 * swap
 * declval
 * type_identity
 */

namespace aria {

template <class T, T v> struct integral_constant {
  using value_type = T;
  static constexpr T value = v;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

template <bool v> using bool_constant = integral_constant<bool, v>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

//----------------- Type relationships -----------------------

template <class T, class U> struct is_same : false_type {};
template <class T> struct is_same<T, T> : true_type {};
template <class T, class U> inline constexpr bool is_same_v = is_same<T, U>::value;

namespace details {
template <class To> inline void foo(To) {}

template <typename From, typename To>
concept convertible = requires(From f) { foo<To>(f); };
} // namespace details

template <class From, class To> struct is_convertible : bool_constant<details::convertible<From, To>> {};
template <class From, class To> inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

template <class T, class U> struct is_base_of : bool_constant<!is_same_v<T, void> && is_convertible_v<U *, T *>> {};
template <class T, class U> inline constexpr bool is_base_of_v = is_base_of<T, U>::value;

template <class T, class... Args> inline constexpr bool is_any_of_v = (is_same_v<T, Args> || ...);

//----------------- Miscellaneous transformations -----------------------
template <class T> struct type_identity {
  using type = T;
};

template <bool B, class T, class F> struct conditional : type_identity<T> {};
template <class T, class F> struct conditional<false, T, F> : type_identity<F> {};
template <bool B, class T, class F> using conditional_t = conditional<B, T, F>::type;

template <class...> using void_t = void;

//----------------- const volatile -----------------------
template <class T> struct is_const : false_type {};
template <class T> struct is_const<const T> : true_type {};
template <class T> inline constexpr bool is_const_v = is_const<T>::value;

template <class T> struct add_const : type_identity<const T> {};
template <class T> using add_const_t = add_const<T>::type;
template <class T> struct add_volatile : type_identity<volatile T> {};
template <class T> using add_volatile_t = add_volatile<T>::type;
template <class T> struct add_cv : type_identity<const volatile T> {};
template <class T> using add_cv_t = add_cv<T>::type;

template <class T> struct remove_const : type_identity<T> {};
template <class T> struct remove_const<const T> : type_identity<T> {};
template <class T> using remove_const_t = remove_const<T>::type;
template <class T> struct remove_volatile : type_identity<T> {};
template <class T> struct remove_volatile<volatile T> : type_identity<T> {};
template <class T> using remove_volatile_t = remove_volatile<T>::type;
template <class T> struct remove_cv : type_identity<T> {};
template <class T> struct remove_cv<const T> : type_identity<T> {};
template <class T> struct remove_cv<volatile T> : type_identity<T> {};
template <class T> struct remove_cv<const volatile T> : type_identity<T> {};
template <class T> using remove_cv_t = remove_cv<T>::type;

//----------------- reference -----------------------
namespace detail {
template <class T> auto try_add_lvalue_ref(int) -> type_identity<T &>;
template <class T> auto try_add_lvalue_ref(...) -> type_identity<T>;
template <class T> auto try_add_rvalue_ref(int) -> type_identity<T &&>;
template <class T> auto try_add_rvalue_ref(...) -> type_identity<T>;
} // namespace detail

template <class T> struct add_lvalue_reference : decltype(detail::try_add_lvalue_ref<T>(0)) {};
template <class T> struct add_rvalue_reference : decltype(detail::try_add_rvalue_ref<T>(0)) {};
template <class T> using add_lvalue_reference_t = add_lvalue_reference<T>::type;
template <class T> using add_rvalue_reference_t = add_rvalue_reference<T>::type;

template <class T> struct remove_reference : type_identity<T> {};
template <class T> struct remove_reference<T &> : type_identity<T> {};
template <class T> struct remove_reference<T &&> : type_identity<T> {};
template <class T> using remove_reference_t = remove_reference<T>::type;

template <class T> struct remove_cvref : type_identity<remove_cv_t<remove_reference_t<T>>> {};
template <class T> using remove_cvref_t = remove_cvref<T>::type;

template <class T> struct is_reference : false_type {};
template <class T> struct is_reference<T &> : true_type {};
template <class T> struct is_reference<T &&> : true_type {};
template <class T> inline constexpr bool is_reference_v = is_reference<T>::value;

template <class T> struct is_lvalue_reference : false_type {};
template <class T> struct is_lvalue_reference<T &> : true_type {};
template <class T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <class T> struct is_rvalue_reference : false_type {};
template <class T> struct is_rvalue_reference<T &&> : true_type {};
template <class T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

//----------------- declval -----------------------
template <typename T> typename add_rvalue_reference_t<T> declval() noexcept {}

//----------------- move, forward------------------
template <class T> constexpr remove_reference_t<T> &&move(T &&t) noexcept { return static_cast<remove_reference_t<T> &&>(t); }

template <typename T> constexpr T &&forward(remove_reference_t<T> &t) noexcept { return static_cast<T &&>(t); }

template <typename T> constexpr T &&forward(remove_reference_t<T> &&t) noexcept {
  static_assert(!is_lvalue_reference_v<T>);
  return static_cast<T &&>(t);
}

//----------------- Primary type categories -----------------------
using std::is_class;
using std::is_class_v;
using std::is_enum;
using std::is_enum_v;
using std::is_trivial;
using std::is_trivial_v;
using std::is_union;
using std::is_union_v;

template <class T> struct is_void : is_same<void, remove_cv_t<T>> {};
template <class T> inline constexpr bool is_void_v = is_void<T>::value;

template <class T>
inline constexpr bool is_integral_v = is_any_of_v<remove_cv_t<T>, bool, char, signed char, unsigned char, short, unsigned short, int,
                                                  unsigned int, long, unsigned long, long long, unsigned long long>;
template <class T> struct is_integral : bool_constant<is_integral_v<T>> {};

template <class T> struct is_pointer : false_type {};
template <class T> struct is_pointer<T *> : true_type {};
template <class T> struct is_pointer<T *const> : true_type {};
template <class T> struct is_pointer<T *volatile> : true_type {};
template <class T> struct is_pointer<T *const volatile> : true_type {};
template <class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;

template <class T> struct is_null_pointer : is_same<remove_cv_t<T>, nullptr_t> {};
template <class T> inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

template <class T> struct is_array : false_type {};
template <class T> struct is_array<T[]> : true_type {};
template <class T, size_t N> struct is_array<T[N]> : true_type {};
template <class T> inline constexpr bool is_array_v = is_array<T>::value;

template <class T> inline constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<T>, float, double, long double>;
template <class T> struct is_floating_point : bool_constant<is_floating_point_v<T>> {};

template <class T> inline constexpr bool is_arithmetic_v = is_integral_v<T> || is_floating_point_v<T>;
template <class T> struct is_arithmetic : bool_constant<is_arithmetic_v<T>> {};

template <class T> struct is_scalar : bool_constant<is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_null_pointer_v<T>> {};
template <class T> inline constexpr bool is_scalar_v = is_scalar<T>::value;

template <class T> struct is_object : bool_constant<is_scalar_v<T> || is_array_v<T> || is_union_v<T> || is_class_v<T>> {};
template <class T> inline constexpr bool is_object_v = is_object<T>::value;

template <class T> constexpr bool is_function_v = !is_const_v<const T> && !is_reference_v<T>;
template <class T> struct is_function : bool_constant<is_function_v<T>> {};

//----------------- is_constructible -----------------------
template <class T>
concept _default_construct = requires { T(); } && requires { T{}; } && requires { ::new T; };
template <class T, class... Args>
concept _constructible = requires { T(declval<Args>()...); };

template <class T> struct is_default_constructible : bool_constant<_default_construct<T>> {};
template <class T> inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
template <class T, class... Args> struct is_constructible : bool_constant<_constructible<T, Args...>> {};
template <class T, class... Args> inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
template <class T> struct is_copy_contructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <class T> inline constexpr bool is_copy_contructible_v = is_copy_contructible<T>::value;
template <class T> struct is_move_contructible : is_constructible<T, add_rvalue_reference_t<T>> {};
template <class T> inline constexpr bool is_move_contructible_v = is_move_contructible<T>::value;

//----------------- is_assignable -----------------
template <class T, class U>
concept _assignable = requires(T t) { t = declval<U>(); };
template <class T, class U> struct is_assignable : bool_constant<_assignable<T, U>> {};
template <class T, class U> inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
template <class T> struct is_copy_assignable : is_assignable<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <class T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
template <class T> struct is_move_assignable : is_assignable<T, add_rvalue_reference_t<T>> {};
template <class T> inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

//----------------- is_destructible -----------------------
template <class T>
concept _destructible = is_lvalue_reference_v<T> || requires { declval<T &>().~T(); };

template <class T>
concept _nothrow_destructible = is_lvalue_reference_v<T> || requires { requires noexcept(declval<T>().~T()); };

template <class T> struct is_destructible : bool_constant<_destructible<T>> {};
template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;
template <class T> struct is_nothrow_destructible : bool_constant<_nothrow_destructible<T>> {};
template <class T> inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

//-----------------swap, is_swappable -----------------------
template <class T>
  requires is_move_contructible_v<T> && is_move_assignable_v<T>
constexpr void swap(T &a, T &b) noexcept {
  T temp = move(a);
  a = move(b);
  b = move(temp);
}

template <class T, class U>
concept _swappable = requires {
  swap(declval<T>(), declval<U>());
  swap(declval<U>(), declval<T>());
};

template <class T, class U>
concept _nothrow_swappable = requires {
  requires noexcept(swap(declval<T>(), declval<U>()));
  requires noexcept(swap(declval<U>(), declval<T>()));
};

template <class T, class U> struct is_swappable_with : bool_constant<_swappable<T, U>> {};
template <class T, class U> inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
template <class T> struct is_swappable : is_swappable_with<T &, T &> {};
template <class T> inline constexpr bool is_swappable_v = is_swappable<T>::value;
template <class T, class U> struct is_nothrow_swappable_with : bool_constant<_nothrow_swappable<T, U>> {};
template <class T, class U> inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
template <class T> struct is_nothrow_swappable : is_nothrow_swappable_with<T &, T &> {};
template <class T> inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

template <class T, size_t N>
void swap(T (&a)[N], T (&b)[N]) noexcept
  requires is_swappable_v<T>
{
  for (int i = 0; i < N; i++)
    swap(a[i], b[i]);
}

//----------------- common_type -----------------------
template <class... Ts> struct common_type {};
template <class... Ts> using common_type_t = common_type<Ts...>::type;
template <class T> struct common_type<T> : type_identity<T> {};

template <class T, class U> struct common_type<T, U> {
  using type = decltype(false ? declval<T>() : declval<U>());
};

template <class T, class U, class... R> struct common_type<T, U, R...> : common_type<common_type_t<T, U>, R...> {};

//----------------- type properties -----------------------
template <class T> struct is_signed : false_type {};
template <class T>
  requires(is_arithmetic_v<T>)
struct is_signed<T> : bool_constant<T(-1) < T(0)> {};
template <class T> inline constexpr bool is_signed_v = is_signed<T>::value;

template <class T> struct is_unsigned : false_type {};
template <class T>
  requires(is_arithmetic_v<T>)
struct is_unsigned<T> : bool_constant<T(0) < T(-1)> {};
template <class T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

} // namespace aria