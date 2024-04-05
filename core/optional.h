#pragma once
#include "utility.h"

namespace aria {

struct nullopt_t {
  struct Tag {};
  constexpr explicit nullopt_t(Tag) {}
};
inline constexpr auto nullopt = nullopt_t{nullopt_t::Tag{}};

class bad_optional_access : public std::exception {
public:
  [[no_discard]] const char *what() const noexcept override { return "Bad optional access"; }
};

template <class T> class optional {
public:
  using value_type = T;
  constexpr optional() noexcept : empty_{} {}
  constexpr optional(nullopt_t) noexcept : empty_{} {}
  constexpr ~optional() { reset(); }

  template <class U = T>
    requires is_constructible_v<T, U &&> && !is_same_v<remove_cvref_t<U>, optional>
             constexpr optional(U && u) : has_value_(true),
  value_{forward<U>(u)} {}

  constexpr optional(const optional &rhs) {
    if (rhs) {
      has_value_ = true;
      construct_in_place(rhs.value());
    }
  }

  constexpr optional(optional &&rhs) {
    if (rhs) {
      has_value_ = true;
      construct_in_place(move(rhs.value()));
    }
  }

  constexpr optional &operator=(nullopt_t) noexcept { reset(); }

  constexpr optional &operator=(const optional &rhs) {
    if (this == &rhs)
      return *this;
    if (!rhs.has_value()) {
      reset();
    } else if (has_value()) {
      value_ = rhs.value(); // copy-assign
    } else {
      construct_in_place(rhs.value()); // in-place construct
      has_value_ = true;
    }
    return *this;
  }

  constexpr optional &operator=(optional &&rhs) {
    if (this == &rhs)
      return *this;
    if (!rhs.has_value()) {
      reset();
    } else if (has_value()) {
      value_ = move(rhs.value()); // move-assign
    } else {
      construct_in_place(move(rhs.value())); // in-place move construct
      has_value_ = true;
    }
    return *this;
  }

  template <class U = T>
    requires is_constructible_v<T, U> && !is_same_v<remove_cvref_t<U>, optional>
                                             constexpr optional &
                                         operator=(U &&value) {
    if (has_value()) {
      value_ = value; // copy-assign
    } else {
      construct_in_place(value); // in-place construct
      has_value_ = true;
    }
    return *this;
  }

  constexpr const T &operator*() const noexcept { return value_; }
  constexpr T &operator*() noexcept { return value_; }
  constexpr const T *operator->() const noexcept { return &value_; }
  constexpr T *operator->() noexcept { return &value_; }

  constexpr operator bool() const noexcept { return has_value_; }
  constexpr bool has_value() const noexcept { return has_value_; }

  template <class Self> constexpr auto &&value(this Self &&self) {
    if (!self.has_value())
      throw(bad_optional_access{});
    return self.value_;
  }

  template <class Self, class U> constexpr auto value_or(this Self &&self, U &&default_value) {
    return self.has_value_ ? self.value_ : static_cast<T>(forward<U>(default_value));
  }

  constexpr void reset() noexcept {
    if constexpr (is_destructible_v<T>) {
      if (has_value_)
        value_.~T();
    }
    has_value_ = false;
  }

  constexpr void swap(optional &rhs) noexcept {
    if (*this && rhs) {
      ::aria::swap(this->value(), rhs.value());
    } else if (*this && !rhs) {
      rhs = move(*this);
      reset();
    } else if (!*this && rhs) {
      *this = move(rhs);
      rhs.reset();
    }
  }

private:
  template <class... Args> constexpr void construct_in_place(Args &&...args) { new (&value_) T(forward<Args>(args)...); }

  struct empty_byte {};
  union {
    empty_byte empty_;
    T value_;
  };
  bool has_value_ = false;
};

template <class T> void swap(optional<T> &a, optional<T> &b) noexcept { a.swap(b); }

} // namespace aria