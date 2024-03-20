#pragma once

/*
* https://en.cppreference.com/w/cpp/header/type_traits
*/

namespace aria {

template <class T, T v>
struct integral_constant {
  using value_type = T;
  static constexpr T value = v;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template<bool v>
using bool_constant = integral_constant<bool, v>;

//----------------- type_identity -----------------------
template <class T> struct type_identity {
  using type = T;
}; 

//----------------- conditional -----------------------
template<bool B, class T, class F>
struct conditional { using type = T; };
 
template<class T, class F>
struct conditional<false, T, F> { using type = F; };

template <bool B, class T, class F>
using conditional_t = conditional<B, T, F>::type;

//----------------- is_same -----------------------
template <class T, class U>
struct is_same : false_type {};

template <class T>
struct is_same<T, T> : true_type {};

template<class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

//----------------- is_convertible -----------------------
namespace details {
template <class To>
inline void foo(To) {}

template <typename From, typename To>
concept convertible = requires(From f) { foo<To>(f); };

}  // namespace detail

template <class From, class To>
struct is_convertible : conditional_t<details::convertible<From, To>, true_type, false_type> {};

template<class From, class To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

//----------------- remove_cv -----------------------

template <class T> struct remove_cv : type_identity<T> {};
template <class T> struct remove_cv<const T> : type_identity<T> {};
template <class T> struct remove_cv<volatile T> : type_identity<T> {};
template <class T> struct remove_cv<const volatile T> : type_identity<T> {};
template <class T> using remove_cv_t = remove_cv<T>::type;

//----------------- is_...type -----------------------

template <class T>
struct is_void : is_same<void, typename remove_cv<T>::type> {};

template <class T>
inline constexpr bool is_void_v = is_void<T>::value;

//----------------- add_lvalue_reference, add_rvalue_reference -----------------------

namespace detail {

template <class T>  // Note that “cv void&” is a substitution failure
auto try_add_lvalue_reference(int) -> type_identity<T&>;
template <class T>  // Handle T = cv void case
auto try_add_lvalue_reference(...) -> type_identity<T>;

template <class T>
auto try_add_rvalue_reference(int) -> type_identity<T&&>;
template <class T>
auto try_add_rvalue_reference(...) -> type_identity<T>;
}  // namespace detail

template <class T>
struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0)) {
};

template <class T>
struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0)) {
};

template<class T>
using add_lvalue_reference_t = add_lvalue_reference<T>::type;

template< class T >
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

//----------------- is_base_of -----------------------
template <class T, class U>
struct is_base_of : bool_constant<!is_void_v<T> && is_convertible_v<U*, T*>> {};

template <class T, class U>
inline constexpr bool is_base_of_v = is_base_of<T, U>::value;

//----------------- remove_reference -----------------------
template <class T> struct remove_reference : type_identity<T> {};
template <class T> struct remove_reference<T &> : type_identity<T> {};
template <class T> struct remove_reference<T &&> : type_identity<T> {};
template <class T> using remove_reference_t = remove_reference<T>::type;

//----------------- is_lvalue_reference is_rvalue_reference-----------------------
template <class T> struct is_lvalue_reference : false_type {};
template <class T> struct is_lvalue_reference<T &> : true_type {};
template <class T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <class T> struct is_rvalue_reference : false_type {};
template <class T> struct is_rvalue_reference<T &&> : true_type {};
template <class T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

//----------------- declval -----------------------
template <typename T> typename add_rvalue_reference_t<T> declval() noexcept {}

//----------------- is_default_constructible -----------------------
namespace details {
template <class T>
concept default_construct = requires {
  T();
  T{};
  ::new T;
};
}

template <class T>
struct is_default_constructible : bool_constant<details::default_construct<T>> {
};

template <class T>
inline constexpr bool is_default_constructible_v =
    is_default_constructible<T>::value;

}  // namespace aria