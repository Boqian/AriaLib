#pragma once

/*
 * https://en.cppreference.com/w/cpp/header/type_traits
 */

namespace aria {

template <class T, T v> struct integral_constant {
  using value_type = T;
  static constexpr T value = v;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <bool v> using bool_constant = integral_constant<bool, v>;

//----------------- type_identity -----------------------
template <class T> struct type_identity {
  using type = T;
};

//----------------- conditional -----------------------
template <bool B, class T, class F> struct conditional {
  using type = T;
};

template <class T, class F> struct conditional<false, T, F> {
  using type = F;
};

template <bool B, class T, class F> using conditional_t = conditional<B, T, F>::type;

//----------------- is_same -----------------------
template <class T, class U> struct is_same : false_type {};

template <class T> struct is_same<T, T> : true_type {};

template <class T, class U> inline constexpr bool is_same_v = is_same<T, U>::value;

// is_any_of
template <class T, class... Args> inline constexpr bool is_any_of_v = (is_same_v<T, Args> || ...);

//----------------- is_convertible -----------------------
namespace details {
template <class To> inline void foo(To) {}

template <typename From, typename To>
concept convertible = requires(From f) { foo<To>(f); };
} // namespace details

template <class From, class To> struct is_convertible : conditional_t<details::convertible<From, To>, true_type, false_type> {};

template <class From, class To> inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

//----------------- add_const add_volatile add_cv-----------------------
template <class T> struct add_const : type_identity<const T> {};
template <class T> using add_const_t = add_const<T>::type;
template <class T> struct add_volatile : type_identity<volatile T> {};
template <class T> using add_volatile_t = add_volatile<T>::type;
template <class T> struct add_cv : type_identity<const volatile T> {};
template <class T> using add_cv_t = add_cv<T>::type;

//----------------- remove_const remove_volatile remove_cv-----------------------
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

//----------------- Primary type categories -----------------------

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

template <class T> struct is_array : false_type {};
template <class T> struct is_array<T[]> : true_type {};
template <class T, size_t N> struct is_array<T[N]> : true_type {};
template <class T> inline constexpr bool is_array_v = is_array<T>::value;

template <class T> inline constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<T>, float, double, long double>;
template <class T> struct is_floating_point : bool_constant<is_floating_point_v<T>> {};

template <class T> inline constexpr bool is_arithmetic_v = is_integral_v<T> || is_floating_point_v<T>;
template <class T> struct is_arithmetic : bool_constant<is_arithmetic_v<T>> {};

//----------------- is_base_of -----------------------
template <class T, class U> struct is_base_of : bool_constant<!is_void_v<T> && is_convertible_v<U *, T *>> {};

template <class T, class U> inline constexpr bool is_base_of_v = is_base_of<T, U>::value;

//----------------- add_lvalue_reference, add_rvalue_reference

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

//----------------- remove_reference -----------------------
template <class T> struct remove_reference : type_identity<T> {};
template <class T> struct remove_reference<T &> : type_identity<T> {};
template <class T> struct remove_reference<T &&> : type_identity<T> {};
template <class T> using remove_reference_t = remove_reference<T>::type;

//----------------- remove_cvref -----------------------
template <class T> struct remove_cvref : type_identity<remove_cv_t<remove_reference_t<T>>> {};
template <class T> using remove_cvref_t = remove_cvref<T>::type;

//----------------- is_lvalue_reference is_rvalue_reference-----------------------
template <class T> struct is_reference : false_type {};
template <class T> struct is_reference<T &> : true_type {};
template <class T> struct is_reference<T &&> : true_type {};
template <class T> inline constexpr bool is_referenct_v = is_reference<T>::value;

template <class T> struct is_lvalue_reference : false_type {};
template <class T> struct is_lvalue_reference<T &> : true_type {};
template <class T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <class T> struct is_rvalue_reference : false_type {};
template <class T> struct is_rvalue_reference<T &&> : true_type {};
template <class T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

//----------------- declval -----------------------
template <typename T> typename add_rvalue_reference_t<T> declval() noexcept {}

//----------------- is_constructible -----------------------
template <class T>
concept _default_construct = requires {
  T();
  T{};
  ::new T;
};

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
concept _assignable = requires { declval<T>() = declval<U>(); };
template <class T, class U> struct is_assignable : bool_constant<_assignable<T, U>> {};
template <class T, class U> inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
template <class T> struct is_copy_assignable : is_assignable<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <class T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
template <class T> struct is_move_assignable : is_assignable<T, add_rvalue_reference_t<T>> {};
template <class T> inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

//----------------- is_destructible -----------------------
template <class T>
concept _destructible = is_lvalue_reference_v<T> || requires { declval<T>().~T(); };
template <class T> struct is_destructible : bool_constant<_destructible<T>> {};
template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;

//----------------- common_type -----------------------
template <class... Ts> struct common_type {};
template <class... Ts> using common_type_t = common_type<Ts...>::type;
template <class T> struct common_type<T> : type_identity<T> {};

template <class T, class U> struct common_type<T, U> {
  using type = decltype(false ? declval<T>() : declval<U>());
};

template <class T, class U, class... R> struct common_type<T, U, R...> : common_type<common_type_t<T, U>, R...> {};

//----------------- type properties -----------------------
template <class T> struct is_const : false_type {};
template <class T> struct is_const<const T> : true_type {};
template <class T> inline constexpr bool is_const_v = is_const<T>::value;

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