#pragma once
#include "cstddef.h"
#include <type_traits> //for complier implemented traits
// https://en.cppreference.com/w/cpp/header/type_traits

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

namespace _traits {
template <class To> inline void foo(To) {}

template <typename From, typename To> concept convertible = requires(From f) { foo<To>(f); };
} // namespace _traits

template <class From, class To> struct is_convertible : bool_constant<_traits::convertible<From, To>> {};
template <class From, class To> inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

template <class T, class U> struct is_base_of : bool_constant<!is_same_v<T, void> && is_convertible_v<U *, T *>> {};
template <class T, class U> inline constexpr bool is_base_of_v = is_base_of<T, U>::value;

template <class T, class... Args> inline constexpr bool is_any_of_v = (is_same_v<T, Args> || ...);

namespace _traits {
template <class T, class U> concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <class Fn, class... Args> concept invocable = requires(Fn fn, Args... args) { fn(forward<Args>(args)...); };

template <class Fn, class R, class... Args> concept invocable_r = requires(Fn fn, Args... args) {
  { fn(forward<Args>(args)...) } -> same_as<R>;
};

} // namespace _traits

template <class Fn, class... Args> struct is_invocable : bool_constant<_traits::invocable<Fn, Args...>> {};
template <class Fn, class... Args> inline constexpr bool is_invocable_v = is_invocable<Fn, Args...>::value;
template <class Fn, class R, class... Args> struct is_invocable_r : bool_constant<_traits::invocable_r<Fn, R, Args...>> {};
template <class Fn, class R, class... Args> inline constexpr bool is_invocable_r_v = is_invocable_r<Fn, R, Args...>::value;

template <class Fn, class... Args> struct invoke_result {};
template <class Fn, class... Args> requires is_invocable_v<Fn, Args...> struct invoke_result<Fn, Args...> {
  using type = decltype(declval<Fn>()(declval<Args>()...));
};

template <class Fn, class... Args> using invoke_result_t = invoke_result<Fn, Args...>::type;

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
namespace _traits {
template <class T> auto try_add_lvalue_ref(int) -> type_identity<T &>;
template <class T> auto try_add_lvalue_ref(...) -> type_identity<T>;
template <class T> auto try_add_rvalue_ref(int) -> type_identity<T &&>;
template <class T> auto try_add_rvalue_ref(...) -> type_identity<T>;
} // namespace _traits

template <class T> struct add_lvalue_reference : decltype(_traits::try_add_lvalue_ref<T>(0)) {};
template <class T> struct add_rvalue_reference : decltype(_traits::try_add_rvalue_ref<T>(0)) {};
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

//----------------- pointer -----------------------
namespace _traits {
template <class T> auto try_add_pointer(int) -> type_identity<remove_reference_t<T> *>;
template <class T> auto try_add_pointer(...) -> type_identity<T>;
} // namespace _traits

template <class T> struct add_pointer : decltype(_traits::try_add_pointer<T>(0)) {};
template <class T> using add_pointer_t = typename add_pointer<T>::type;

template <class T> struct remove_pointer : type_identity<T> {};
template <class T> struct remove_pointer<T *> : type_identity<T> {};
template <class T> struct remove_pointer<T *const> : type_identity<T> {};
template <class T> struct remove_pointer<T *volatile> : type_identity<T> {};
template <class T> struct remove_pointer<T *const volatile> : type_identity<T> {};
template <class T> using remove_pointer_t = typename remove_pointer<T>::type;

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

template <class T> constexpr bool is_function_v = !is_const_v<const T> && !is_reference_v<T>;
template <class T> struct is_function : bool_constant<is_function_v<T>> {};

template <class T> struct _is_member_object_pointer : false_type {};
template <class T, class U> struct _is_member_object_pointer<T U::*> : bool_constant<!is_function_v<T>> {};
template <class T> struct is_member_object_pointer : _is_member_object_pointer<remove_cv_t<T>> {};
template <class T> inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
template <class T> struct _is_member_function_pointer : false_type {};
template <class T, class U> struct _is_member_function_pointer<T U::*> : bool_constant<is_function_v<T>> {};
template <class T> struct is_member_function_pointer : _is_member_function_pointer<remove_cv_t<T>> {};
template <class T> inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

//----------------- Composite type categories -----------------------
template <class T> constexpr bool is_fundamental_v = is_arithmetic_v<T> || is_void_v<T> || is_null_pointer_v<T>;
template <class T> struct is_fundamental : bool_constant<is_fundamental_v<T>> {};

template <class T> struct is_compound : bool_constant<is_fundamental_v<T>> {};
template <class T> inline constexpr bool is_compound_v = is_compound<T>::value;

template <class T> struct is_scalar : bool_constant<is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_null_pointer_v<T>> {};
template <class T> inline constexpr bool is_scalar_v = is_scalar<T>::value;

template <class T> struct is_object : bool_constant<is_scalar_v<T> || is_array_v<T> || is_union_v<T> || is_class_v<T>> {};
template <class T> inline constexpr bool is_object_v = is_object<T>::value;

template <class T> constexpr bool is_member_pointer_v = is_member_object_pointer_v<T> || is_member_function_pointer_v<T>;
template <class T> struct is_member_pointer : bool_constant<is_member_pointer_v<T>> {};

//----------------- Operations on traits -----------------------
template <class B> struct negation : bool_constant<!bool(B::value)> {};
template <class B> inline constexpr bool negation_v = negation<B>::value;

template <class...> struct conjunction : std::true_type {};
template <class B1> struct conjunction<B1> : B1 {};
template <class B1, class... Bn> struct conjunction<B1, Bn...> : conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};
template <class... B> inline constexpr bool conjunction_v = conjunction<B...>::value;

template <class... B> struct disjunction : false_type {};
template <class B1> struct disjunction<B1> : B1 {};
template <class B1, class... Bn> struct disjunction<B1, Bn...> : conditional_t<bool(B1::value), B1, disjunction<Bn...>> {};
template <class... B> inline constexpr bool disjunction_v = conjunction<B...>::value;

//----------------- type properties -----------------------
using std::has_unique_object_representations_v;
using std::is_abstract;
using std::is_abstract_v;
using std::is_aggregate;
using std::is_aggregate_v;
using std::is_empty;
using std::is_empty_v;
using std::is_final;
using std::is_final_v;
using std::is_standard_layout;
using std::is_standard_layout_v;
using std::is_trivial;
using std::is_trivial_v;
using std::is_trivially_copyable;
using std::is_trivially_copyable_v;

template <class T> struct is_signed : false_type {};
template <class T> requires(is_arithmetic_v<T>) struct is_signed<T> : bool_constant<T(-1) < T(0)> {};
template <class T> inline constexpr bool is_signed_v = is_signed<T>::value;

template <class T> struct is_unsigned : false_type {};
template <class T> requires(is_arithmetic_v<T>) struct is_unsigned<T> : bool_constant<T(0) < T(-1)> {};
template <class T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

template <class T> struct is_bounded_array : false_type {};
template <class T, size_t N> struct is_bounded_array<T[N]> : true_type {};
template <class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

template <class T> struct is_unbounded_array : false_type {};
template <class T> struct is_unbounded_array<T[]> : true_type {};
template <class T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

//----------------- Sign modifiers -----------------------
template <class T> struct make_unsigned {};
template <class T> requires is_unsigned_v<T> struct make_unsigned<T> : type_identity<T> {};
template <> struct make_unsigned<char> : type_identity<unsigned char> {};
template <> struct make_unsigned<short> : type_identity<unsigned short> {};
template <> struct make_unsigned<int> : type_identity<unsigned int> {};
template <> struct make_unsigned<long> : type_identity<unsigned long> {};
template <> struct make_unsigned<long long> : type_identity<unsigned long long> {};
template <class T> using make_unsigned_t = make_unsigned<T>::type;

template <class T> struct make_signed {};
template <class T> requires is_signed_v<T> struct make_signed<T> : type_identity<T> {};
template <> struct make_signed<unsigned char> : type_identity<char> {};
template <> struct make_signed<unsigned short> : type_identity<short> {};
template <> struct make_signed<unsigned int> : type_identity<int> {};
template <> struct make_signed<unsigned long> : type_identity<long> {};
template <> struct make_signed<unsigned long long> : type_identity<long long> {};
template <class T> using make_signed_t = make_signed<T>::type;

//----------------- is_constructible -----------------------
template <class T> concept _default_construct = requires { T(); } && requires { T{}; } && requires { ::new T; };
template <class T, class... Args> concept _constructible = requires { T(declval<Args>()...); };

template <class T> struct is_default_constructible : bool_constant<_default_construct<T>> {};
template <class T> inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
template <class T, class... Args> struct is_constructible : bool_constant<_constructible<T, Args...>> {};
template <class T, class... Args> inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
template <class T> struct is_copy_contructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <class T> inline constexpr bool is_copy_constructible_v = is_copy_contructible<T>::value;
template <class T> struct is_move_contructible : is_constructible<T, add_rvalue_reference_t<T>> {};
template <class T> inline constexpr bool is_move_constructible_v = is_move_contructible<T>::value;

//----------------- is_assignable -----------------
template <class T, class U> concept _assignable = requires(T t) { t = declval<U>(); };
template <class T, class U> struct is_assignable : bool_constant<_assignable<T, U>> {};
template <class T, class U> inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
template <class T> struct is_copy_assignable : is_assignable<T, add_lvalue_reference_t<add_const_t<T>>> {};
template <class T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
template <class T> struct is_move_assignable : is_assignable<T, add_rvalue_reference_t<T>> {};
template <class T> inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

//----------------- is_destructible -----------------------
template <class T> concept _destructible = is_lvalue_reference_v<T> || requires { declval<T &>().~T(); };

template <class T> concept _nothrow_destructible = is_lvalue_reference_v<T> || requires { requires noexcept(declval<T>().~T()); };

template <class T> struct is_destructible : bool_constant<_destructible<T>> {};
template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;
template <class T> struct is_nothrow_destructible : bool_constant<_nothrow_destructible<T>> {};
template <class T> inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

//-----------------swap, is_swappable -----------------------
template <class T> requires is_move_constructible_v<T> && is_move_assignable_v<T> constexpr void swap(T &a, T &b) noexcept {
  T temp = move(a);
  a = move(b);
  b = move(temp);
}

template <class T, class U> concept _swappable = requires {
  swap(declval<T>(), declval<U>());
  swap(declval<U>(), declval<T>());
};

template <class T, class U> concept _nothrow_swappable = requires {
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

template <class T, size_t N> void swap(T (&a)[N], T (&b)[N]) noexcept requires is_swappable_v<T> {
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

//----------------- Arrays -----------------------
template <class T> struct remove_extent : type_identity<T> {};
template <class T, size_t N> struct remove_extent<T[N]> : type_identity<T> {};
template <class T> struct remove_extent<T[]> : type_identity<T> {};
template <class T> using remove_extent_t = typename remove_extent<T>::type;

template <class T> struct remove_all_extents : type_identity<T> {};
template <class T, size_t N> struct remove_all_extents<T[N]> : remove_all_extents<T> {};
template <class T> struct remove_all_extents<T[]> : remove_all_extents<T> {};
template <class T> using remove_all_extents_t = typename remove_all_extents<T>::type;

//----------------- Property queries -----------------------
template <class T, unsigned N = 0> struct extent : integral_constant<size_t, 0> {};
template <class T> struct extent<T[], 0> : integral_constant<size_t, 0> {};
template <class T, unsigned N> struct extent<T[], N> : extent<T, N - 1> {};
template <class T, size_t I> struct extent<T[I], 0> : integral_constant<size_t, I> {};
template <class T, size_t I, unsigned N> struct extent<T[I], N> : extent<T, N - 1> {};
template <class T, unsigned N = 0> inline constexpr size_t extent_v = extent<T, N>::value;

//----------------- decay -----------------------
template <class T> struct decay : remove_cvref<T> {};
template <class T> requires is_array_v<T> struct decay<T> : add_pointer<remove_extent_t<T>> {};
template <class T> requires is_function_v<T> struct decay<T> : add_pointer<T> {};

template <class T> using decay_t = decay<T>::type;

} // namespace aria