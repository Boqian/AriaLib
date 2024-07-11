#pragma once
#include "utility.h"

namespace aria {

//------------------------- construct_at destroy_at -------------------------//

template <class T, class... Args> constexpr T *construct_at(T *p, Args &&...args) {
  return ::new (static_cast<void *>(p)) T(aria::forward<Args>(args)...);
}

template <class T> constexpr void destroy_at(T *p) { p->~T(); }

template <class ForwardIt> constexpr void destroy(ForwardIt first, ForwardIt last) {
  for (; first != last; ++first)
    destroy_at(addressof(*first));
}

template <class ForwardIt, class Size> constexpr ForwardIt destroy_n(ForwardIt first, Size n) {
  for (; n > 0; (void)++first, --n)
    destroy_at(addressof(*first));
  return first;
}

template <class T> concept allocatable = !is_const_v<T> && !is_function_v<T> && !is_reference_v<T>;

//------------------------- aria::allocator -------------------------//

template <allocatable T> struct allocator {
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using propagate_on_container_move_assignment = true_type;

  template <allocatable U> using rebind_alloc = allocator<U>;

  constexpr allocator() noexcept = default;

  // todo operator new is not constexpr
  [[nodiscard]] constexpr T *allocate(size_type n) { return static_cast<T *>(::operator new(n * sizeof(T))); }

  constexpr void deallocate(T *p, size_type n) { ::operator delete(p, n); }
};

template <class T1, class T2> constexpr bool operator==(const allocator<T1> &lhs, const allocator<T2> &rhs) noexcept { return true; }

//------------------------- allocator_traits -------------------------//

template <class Alloc> struct _get_pointer_type : type_identity<typename Alloc::value_type *> {};
template <class Alloc> requires requires { typename Alloc::pointer; }
struct _get_pointer_type<Alloc> : type_identity<typename Alloc::pointer> {};

template <class Alloc> struct _get_const_pointer_type {
  using value_type = typename Alloc::value_type;
  using pointer = typename _get_pointer_type<Alloc>::type;
  using pointer_traits = pointer_traits<pointer>;
  using type = typename pointer_traits::template rebind<const value_type>;
};
template <class Alloc> requires requires { typename Alloc::const_pointer; }
struct _get_const_pointer_type<Alloc> : type_identity<typename Alloc::const_pointer> {};

template <class Alloc> struct _get_void_pointer_type {
  using pointer = typename _get_pointer_type<Alloc>::type;
  using pointer_traits = pointer_traits<pointer>;
  using type = typename pointer_traits::template rebind<void>;
};
template <class Alloc> requires requires { typename Alloc::void_pointer; }
struct _get_void_pointer_type<Alloc> : type_identity<typename Alloc::void_pointer> {};

template <class Alloc> struct _get_const_void_pointer_type {
  using pointer = typename _get_pointer_type<Alloc>::type;
  using pointer_traits = pointer_traits<pointer>;
  using type = typename pointer_traits::template rebind<const void>;
};
template <class Alloc> requires requires { typename Alloc::const_void_pointer; }
struct _get_const_void_pointer_type<Alloc> : type_identity<typename Alloc::const_void_pointer> {};

template <class Alloc> struct _get_difference_type {
  using pointer = typename _get_pointer_type<Alloc>::type;
  using type = typename pointer_traits<pointer>::difference_type;
};
template <class Alloc> requires requires { typename Alloc::difference_type; }
struct _get_difference_type<Alloc> : type_identity<typename Alloc::difference_type> {};

template <class Alloc, class difference_type> struct _get_size_type {
  using type = make_unsigned_t<difference_type>;
};
template <class Alloc, class difference_type> requires requires { typename Alloc::size_type; }
struct _get_size_type<Alloc, difference_type> : type_identity<typename Alloc::size_type> {};

template <class Alloc> struct propagate_on_copy : type_identity<false_type> {};
template <class Alloc> requires requires { typename Alloc::propagate_on_container_copy_assignment; }
struct propagate_on_copy<Alloc> : type_identity<typename Alloc::propagate_on_container_copy_assignment> {};

template <class Alloc> struct propagate_on_move : type_identity<false_type> {};
template <class Alloc> requires requires { typename Alloc::propagate_on_container_move_assignment; }
struct propagate_on_move<Alloc> : type_identity<typename Alloc::propagate_on_container_move_assignment> {};

template <class Alloc> struct propagate_on_swap : type_identity<false_type> {};
template <class Alloc> requires requires { typename Alloc::propagate_on_container_swap; }
struct propagate_on_swap<Alloc> : type_identity<typename Alloc::propagate_on_container_swap> {};

template <class Alloc> struct is_always_equal : type_identity<typename is_empty<Alloc>::type> {};
template <class Alloc> requires requires { typename Alloc::is_always_equal; }
struct is_always_equal<Alloc> : type_identity<typename Alloc::is_always_equal> {};

template <class Newfirst, class T> struct _replace_first_parameter;
template <class Newfirst, template <class, class...> class T, class First, class... Rest>
struct _replace_first_parameter<Newfirst, T<First, Rest...>> {
  using type = T<Newfirst, Rest...>;
};

template <class Newfirst, class Alloc> struct _get_rebind_type : type_identity<typename _replace_first_parameter<Newfirst, Alloc>::type> {};
template <class Newfirst, class Alloc> requires requires { typename Alloc::template rebind_alloc<Newfirst>; }
struct _get_rebind_type<Newfirst, Alloc> : type_identity<typename Alloc::template rebind_alloc<Newfirst>> {};

template <class Alloc> struct allocator_traits {
  using allocator_type = Alloc;
  using value_type = Alloc::value_type;
  using pointer = typename _get_pointer_type<Alloc>::type;
  using const_pointer = typename _get_const_pointer_type<Alloc>::type;
  using void_pointer = typename _get_void_pointer_type<Alloc>::type;
  using const_void_pointer = typename _get_const_void_pointer_type<Alloc>::type;
  using difference_type = typename _get_difference_type<Alloc>::type;
  using size_type = typename _get_size_type<Alloc, difference_type>::type;
  using propagate_on_container_copy_assignment = typename propagate_on_copy<Alloc>::type;
  using propagate_on_container_move_assignment = typename propagate_on_move<Alloc>::type;
  using propagate_on_container_swap = typename propagate_on_swap<Alloc>::type;
  using is_always_equal = typename is_always_equal<Alloc>::type;
  template <class U> using rebind_alloc = typename _get_rebind_type<U, Alloc>::type;

  [[nodiscard]] static constexpr pointer allocate(Alloc &a, size_type n) { return a.allocate(n); }
  static constexpr void deallocate(Alloc &a, pointer p, size_type n) { a.deallicate(p, n); }

  template <class T, class... Args> static constexpr void construct(Alloc &a, T *p, Args &&...args) {
    if constexpr (requires { a.construct(p, forward<Args>(args)...); }) {
      a.construct(p, forward<Args>(args)...);
    } else {
      construct_at(p, forward<Args>(args)...);
    }
  }

  template <class T, class... Args> static constexpr void destroy(Alloc &a, T *p) {
    if constexpr (requires { a.destory(p); }) {
      a.destory(p);
    } else {
      destory_at(p);
    }
  }
};

} // namespace aria