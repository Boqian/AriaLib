#pragma once
#include "utility.h"

namespace aria {

template <class T, class... Args> constexpr T *construct_at(T *p, Args &&...args) {
  return ::new (static_cast<void *>(p)) T(aria::forward<Args>(args)...);
}

template <class T> constexpr void destroy_at(T *p) { p->~T(); }

template <class T>
concept allocatable = !is_const_v<T> && !is_function_v<T> && !is_reference_v<T>;

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

template <class Alloc> struct _get_pointer_type : type_identity<typename Alloc::value_type *> {};
template <class Alloc>
  requires requires { typename Alloc::pointer; }
struct _get_pointer_type<Alloc> : type_identity<typename Alloc::pointer> {};

template <class Alloc> struct _get_const_pointer_type : type_identity<typename const Alloc::value_type *> {};
template <class Alloc>
  requires requires { typename Alloc::const_pointer; }
struct _get_const_pointer_type<Alloc> : type_identity<typename Alloc::const_pointer> {};

template <class Alloc> struct allocator_traits {
  using allocator_type = Alloc;
  using value_type = Alloc::value_type;
  using pointer = typename _get_pointer_type<Alloc>::type;
  using const_pointer = typename _get_const_pointer_type<Alloc>::type;
};

} // namespace aria