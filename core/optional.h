#pragma once
#include "type_traits.h"


namespace aria {

struct nullopt_t {
  struct Tag {};
  constexpr explicit nullopt_t(Tag) {}
};
inline constexpr auto nullopt = nullopt_t{nullopt_t::Tag{}};

template <class T> class optional {
public:
  using value_type = T;
  constexpr optional() noexcept : empty{} {}
  constexpr optional(nullopt_t) noexcept : empty{} {}
  constexpr ~optional() {
    if constexpr (is_destructible_v<T>) {
      if (has_value)
        value.~T();
    }
  }

  template <class U = T>
    requires is_constructible_v<T, U&&>
  constexpr optional(U &&u) : has_value(true), value{forward<U>(u)} {}

  constexpr const T& operator*() const noexcept { return value; }
  constexpr T& operator*() noexcept { return value; }
  constexpr const T* operator->() const noexcept { return &value; }
  constexpr T *operator->() noexcept { return &value; }

  constexpr operator bool() const noexcept { return has_value; }

private:
  struct empty_byte {};
  union {
    empty_byte empty;
    T value;
  };
  bool has_value = false;
};

}