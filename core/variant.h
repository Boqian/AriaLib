#pragma once
#include "allocator.h"
#include "exception.h"
#include "utility.h"

namespace aria {

class bad_variant_access : public std::exception {
public:
  bad_variant_access() noexcept = default;
  const char *what() const noexcept override { return "bad variant access"; }
};

inline constexpr size_t variant_npos = -1;

template <class... Ts> struct _variant_storage {};
template <class First, class... Rest> struct _variant_storage<First, Rest...> {
  static constexpr size_t size = 1 + sizeof...(Rest);
  union {
    remove_cv_t<First> head;
    _variant_storage<Rest...> tail;
  };

  template <class... Args> constexpr _variant_storage(integral_constant<size_t, 0>, Args &&...args) : head(forward<Args &&>(args)...) {}
  template <size_t I, class... Args>
    requires(I > 0)
  constexpr _variant_storage(integral_constant<size_t, I>, Args &&...args)
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
  static_assert(I < 32, "reached maximum");
  if constexpr (I == 0) {
    return static_cast<Storage &&>(obj).get();
  } else {
    return _variant_raw_get<I - 1>(static_cast<Storage &&>(obj).tail);
  }
}

template <class Storage, class Func> decltype(auto) visit_with_index(Func &func, Storage &storage, size_t index) {
  switch (index) {
  case (1):
    return Func(_variant_raw_get<1>(storage));
  case (2):
    return Func(_variant_raw_get<2>(storage));
  case (3):
    return Func(_variant_raw_get<3>(storage));
  case (4):
    return Func(_variant_raw_get<4>(storage));
  case (5):
    return Func(_variant_raw_get<5>(storage));
  default:
    throw(bad_variant_access{});
  }
}

template <class... Ts> class variant : _variant_storage<Ts...> {
public:
  using Storage = _variant_storage<Ts...>;
  constexpr variant() noexcept = default;

  ~variant() {}

  constexpr size_t index() const noexcept { return current; }

private:
  template <size_t I> void destroy() { destroy_at(&_variant_raw_get<I>(*this)); }

  void destory() {}

  size_t current = variant_npos;
};
} // namespace aria