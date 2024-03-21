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
  constexpr optional() noexcept : empty_{} {}
  constexpr optional(nullopt_t) noexcept : empty_{} {}
  constexpr ~optional() {
    if constexpr (is_destructible_v<T>) {
      if (has_value_)
        value_.~T();
    }
  }

  template <class U = T>
    requires is_constructible_v<T, U&&>
  constexpr optional(U &&u) : has_value_(true), value_{forward<U>(u)} {}

  constexpr const T& operator*() const noexcept { return value_; }
  constexpr T& operator*() noexcept { return value_; }
  constexpr const T* operator->() const noexcept { return &value_; }
  constexpr T *operator->() noexcept { return &value_; }

  constexpr operator bool() const noexcept { return has_value_; }
  constexpr bool has_value() const noexcept { return has_value_; }

  template <class Self> constexpr auto& value(this Self &&self) {
    return self.value_;
  }

  template <class Self, class U>
  constexpr auto value_or(this Self &&self, U &&default_value) {
    return self.has_value_ ? self.value_
                           : static_cast<T>(forward<U>(default_value));
  }

private:
  struct empty_byte {};
  union {
    empty_byte empty_;
    T value_;
  };
  bool has_value_ = false;
};

}