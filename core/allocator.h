#pragma once
#include "utility.h"

namespace aria {

//------------------------- construct_at destroy_at -------------------------//

template <class T, class... Args> constexpr T *construct_at(T *p, Args &&...args) {
  return ::new (static_cast<void *>(p)) T(aria::forward<Args>(args)...);
}

template <class T> constexpr void destroy_at(T *p) { p->~T(); }

template <class T>
concept allocatable = !is_const_v<T> && !is_function_v<T> && !is_reference_v<T>;

//------------------------- aria::allocator -------------------------//

template <allocatable T> struct allocator {
  using size_type = unsigned int;
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = const value_type *;

  template <class U> using rebind_alloc = allocator<U>;

  constexpr allocator() noexcept = default;

  // todo operator new is not constexpr
  [[nodiscard]] constexpr pointer allocate(size_type n) { return static_cast<pointer>(::operator new(n * sizeof(T))); }

  constexpr void deallocate(T *p, size_type n) { ::operator delete(p, n); }
};

//------------------------- allocator_traits -------------------------//

template <class Alloc> struct _get_pointer_type : type_identity<typename Alloc::value_type *> {};
template <class Alloc>
  requires requires { typename Alloc::pointer; }
struct _get_pointer_type<Alloc> : type_identity<typename Alloc::pointer> {};

template <class Alloc> struct _get_const_pointer_type {
  using value_type = typename Alloc::value_type;
  using pointer = typename _get_pointer_type<Alloc>::type;
  using pointer_traits = pointer_traits<pointer>;
  using type = typename pointer_traits::template rebind<const value_type>;
};
template <class Alloc>
  requires requires { typename Alloc::const_pointer; }
struct _get_const_pointer_type<Alloc> : type_identity<typename Alloc::const_pointer> {};

template <class Alloc> struct _get_void_pointer_type {
  using pointer = typename _get_pointer_type<Alloc>::type;
  using pointer_traits = pointer_traits<pointer>;
  using type = typename pointer_traits::template rebind<void>;
};
template <class Alloc>
  requires requires { typename Alloc::void_pointer; }
struct _get_void_pointer_type<Alloc> : type_identity<typename Alloc::void_pointer> {};

template <class Alloc> struct _get_const_void_pointer_type {
  using pointer = typename _get_pointer_type<Alloc>::type;
  using pointer_traits = pointer_traits<pointer>;
  using type = typename pointer_traits::template rebind<const void>;
};
template <class Alloc>
  requires requires { typename Alloc::const_void_pointer; }
struct _get_const_void_pointer_type<Alloc> : type_identity<typename Alloc::const_void_pointer> {};

template <class Alloc> struct _get_difference_type {
  using pointer = typename _get_pointer_type<Alloc>::type;
  using type = typename pointer_traits<pointer>::difference_type;
};
template <class Alloc>
  requires requires { typename Alloc::difference_type; }
struct _get_difference_type<Alloc> : type_identity<typename Alloc::difference_type> {};

template <class Alloc, class difference_type> struct _get_size_type {
  using type = make_unsigned_t<difference_type>;
};
template <class Alloc, class difference_type>
  requires requires { typename Alloc::size_type; }
struct _get_size_type<Alloc, difference_type> : type_identity<typename Alloc::size_type> {};

template <class Alloc> struct allocator_traits {
  using allocator_type = Alloc;
  using value_type = Alloc::value_type;
  using pointer = typename _get_pointer_type<Alloc>::type;
  using const_pointer = typename _get_const_pointer_type<Alloc>::type;
  using void_pointer = typename _get_void_pointer_type<Alloc>::type;
  using const_void_pointer = typename _get_const_void_pointer_type<Alloc>::type;
  using difference_type = typename _get_difference_type<Alloc>::type;
  using size_type = typename _get_size_type<Alloc, difference_type>::type;
};

} // namespace aria