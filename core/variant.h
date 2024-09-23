#pragma once
#include "allocator.h"
#include "stdexcept.h"
#include "type_list.h"
#include "utility.h"

namespace aria {

inline constexpr size_t variant_npos = -1;

class bad_variant_access : public exception {
public:
  bad_variant_access() noexcept = default;
  const char *what() const noexcept override { return "bad variant access"; }
};

namespace _variant {

template <class... Ts> struct storage {
  static constexpr size_t size = sizeof...(Ts);
};
template <class First, class... Rest> struct storage<First, Rest...> {
  static constexpr size_t size = 1 + sizeof...(Rest);
  union {
    remove_cv_t<First> head;
    storage<Rest...> tail;
  };

  template <class... Args> constexpr storage(integral_constant<size_t, 0>, Args &&...args) : head(forward<Args>(args)...) {}
  template <size_t I, class... Args> requires(I > 0)
  constexpr storage(integral_constant<size_t, I>, Args &&...args) : tail(integral_constant<size_t, I - 1>(), forward<Args>(args)...) {}

  constexpr storage() noexcept {}
  constexpr ~storage() noexcept {}
  constexpr storage(storage &&) = default;
  constexpr storage(const storage &) = default;
  constexpr storage &operator=(storage &&) = default;
  constexpr storage &operator=(const storage &) = default;

  constexpr First &get() & noexcept { return head; }
  constexpr const First &get() const & noexcept { return head; }
  constexpr First &&get() && noexcept { return move(head); }
  constexpr const First &&get() const && noexcept { return move(head); }
};

template <size_t I, class Storage> constexpr decltype(auto) raw_get(Storage &&obj) {
  static_assert(I < 32, "reached maximum");
  static_assert(I < obj.size, "out of range");
  if constexpr (I == 0) {
    return static_cast<Storage &&>(obj).get();
  } else {
    return raw_get<I - 1>(static_cast<Storage &&>(obj).tail);
  }
}

template <class Storage, class Func> constexpr decltype(auto) visit_with_index(Func &func, Storage &storage, size_t index) {
  if constexpr (storage.size == 0) {
    throw(bad_variant_access{});
  } else {
    if (index == 0)
      return func(storage.get());
    else
      return visit_with_index(func, storage.tail, index - 1);
  }
}

template <class S1, class S2, class Func> constexpr decltype(auto) bivisit_with_index(Func &func, S1 &&a, S2 &&b, size_t index) {
  static_assert(same_as<remove_cvref_t<S1>, remove_cvref_t<S2>>);
  if constexpr (a.size == 0) {
    throw(bad_variant_access{});
  } else {
    if (index == 0)
      return func(forward<S1>(a).get(), forward<S2>(b).get());
    else
      return bivisit_with_index(func, forward_like<S1>(a.tail), forward_like<S2>(b.tail), index - 1);
  }
}

} // namespace _variant

//------------------------- overload match detector -------------------------

template <size_t N, class A> struct overload_frag {
  using type = A;
  using arr1 = A[1];
  template <class T> requires requires { arr1{declval<T>()}; } auto operator()(A, T &&) -> overload_frag<N, A>;
};

template <class Seq, class... Args> struct make_overload;

template <size_t... Idx, class... Args> struct make_overload<index_sequence<Idx...>, Args...> : overload_frag<Idx, Args>... {
  using overload_frag<Idx, Args>::operator()...;
};

template <class T, class... Ts>
using best_overload_match = typename decltype(make_overload<make_index_sequence<sizeof...(Ts)>, Ts...>{}(declval<T>(), declval<T>()))::type;

template <class T, class... Ts> concept has_non_ambiguous_match = requires { typename best_overload_match<T, Ts...>; };

//------------------------- variant -------------------------

template <class... Ts> class variant : public _variant::storage<Ts...> {
public:
  using Storage = _variant::storage<Ts...>;
  constexpr variant() requires is_default_constructible_v<nth_type<0, Ts...>> : Storage(integral_constant<size_t, 0>()), which(0) {}

  constexpr ~variant() { destory(); }

  template <class T, class M = best_overload_match<T, Ts...>, size_t Idx = index_of<M, Ts...>()> requires has_non_ambiguous_match<T, Ts...>
  constexpr variant(T &&t) : Storage(integral_constant<size_t, Idx>(), forward<T>(t)), which(Idx) {}

  constexpr variant(const variant &rhs) : which(rhs.which) {
    if (!rhs.valueless_by_exception()) 
      construct_from(rhs.storage(), rhs.index());
  }

  constexpr variant(variant &&rhs) noexcept : which(rhs.which) {
    if (!rhs.valueless_by_exception())
      construct_from(move(rhs.storage()), rhs.index());   
  }

  constexpr size_t index() const noexcept { return which; }
  constexpr bool valueless_by_exception() const noexcept { return which == variant_npos; }

private:
  constexpr Storage &storage() { return *this; }
  constexpr const Storage &storage() const { return *this; }

  constexpr void destory() {
    if (!valueless_by_exception()) {
      auto f = [](auto &x) { destroy_at(&x); };
      _variant::visit_with_index(f, storage(), which);
      which = variant_npos;
    }
  }

  template <class S> void construct_from(S &&src, size_t index) {
    auto f = []<typename T>(auto &dest_head, T &&src_head) { construct_at(&dest_head, forward<T>(src_head)); }; 
    _variant::bivisit_with_index(f, storage(), forward<S>(src), index);
  }

  size_t which = variant_npos;
};

//------------------------- free function -------------------------

template <size_t, class> struct variant_alternative;
template <size_t I, class... Ts> struct variant_alternative<I, variant<Ts...>> {
  using type = nth_type<I, Ts...>;
};
template <size_t I, class... Ts> struct variant_alternative<I, const variant<Ts...>> {
  using type = const nth_type<I, Ts...>;
};
template <size_t I, class T> using variant_alternative_t = typename variant_alternative<I, T>::type;

template <class T, class... Ts> constexpr bool holds_alternative(const variant<Ts...> &v) noexcept {
  static_assert(exact_one_match<T, type_list<Ts...>>());
  return first_match_index<T, type_list<Ts...>>() == v.index();
}

template <size_t I, class... Ts> constexpr auto &get(variant<Ts...> &v) {
  if (I != v.index())
    throw bad_variant_access{};
  return _variant::raw_get<I>(v);
}
template <size_t I, class... Ts> constexpr const auto &get(const variant<Ts...> &v) {
  if (I != v.index())
    throw bad_variant_access{};
  return _variant::raw_get<I>(v);
}
template <size_t I, class... Ts> constexpr auto &&get(variant<Ts...> &&v) {
  if (I != v.index())
    throw bad_variant_access{};
  return _variant::raw_get<I>(move(v));
}
template <size_t I, class... Ts> constexpr const auto &&get(const variant<Ts...> &&v) {
  if (I != v.index())
    throw bad_variant_access{};
  return _variant::raw_get<I>(move(v));
}
template <class T, class... Ts> constexpr T &get(variant<Ts...> &v) { return get<index_of<T, Ts...>()>(v); }
template <class T, class... Ts> constexpr const T &get(const variant<Ts...> &v) { return get<index_of<T, Ts...>()>(v); }
template <class T, class... Ts> constexpr T &&get(variant<Ts...> &&v) { return get<index_of<T, Ts...>()>(move(v)); }
template <class T, class... Ts> constexpr const T &&get(const variant<Ts...> &&v) { return get<index_of<T, Ts...>()>(move(v)); }

} // namespace aria