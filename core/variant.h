#pragma once
#include "utility.h"

namespace aria {

template <class... Ts> struct _variant_storage {};
template <class First, class... Rest> struct _variant_storage<First, Rest...> {
  static constexpr size_t size = 1 + sizeof...(Rest);
  union {
    remove_cv_t<First> head;
    _variant_storage<Rest...> tail;
  };

  template <size_t I, class... Args> constexpr explicit _variant_storage(integral_constant<size_t, I>, Args &&...args);
  template <class... Args>
  constexpr explicit _variant_storage(integral_constant<size_t, 0>, Args &&...args) : head(forward<Args &&>(args)...) {}
  template <size_t I, class... Args>
    requires(I > 0)
  constexpr explicit _variant_storage(integral_constant<size_t, I>, Args &&...args)
      : tail(integral_constant<size_t, I - 1>(), forward<Args &&>(args)...) {}

  constexpr _variant_storage() noexcept {}
  constexpr ~_variant_storage() noexcept {}
  _variant_storage(_variant_storage &&) = default;
  _variant_storage(const _variant_storage &) = default;
  _variant_storage &operator=(_variant_storage &&) = default;
  _variant_storage &operator=(const _variant_storage &) = default;

  constexpr First &get() & noexcept { return head; }
  constexpr const First &get() const & noexcept { return head; }
  constexpr First &&get() && noexcept { return move(head); }
  constexpr const First &&get() const && noexcept { return move(head); }
};

template <size_t I, class Storage> constexpr decltype(auto) _variant_raw_get(Storage &&obj) {
  static_assert(I < 256, "reached maximum");
  if constexpr (I == 0) {
    return static_cast<Storage &&>(obj).get();
  } else {
    return _variant_raw_get<I - 1>(static_cast<Storage &&>(obj).tail);
  }
}

template <class... Ts> class variant {};
} // namespace aria