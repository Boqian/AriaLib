#pragma once
#include "allocator.h"
#include "functional.h"
#include "stdexcept.h"
#include "utility.h"

namespace aria {

struct nullopt_t {
  struct Tag {};
  constexpr explicit nullopt_t(Tag) {}
};
inline constexpr auto nullopt = nullopt_t{nullopt_t::Tag{}};

class bad_optional_access : public exception {
public:
  [[nodiscard]] const char *what() const noexcept override { return "Bad optional access"; }
};

template <class T> class optional;

template <class T> struct is_optional : false_type {};
template <class T> struct is_optional<optional<T>> : true_type {};
template <class T> inline constexpr bool is_optional_v = is_optional<T>::value;

template <class T> class optional {
public:
  using value_type = T;
  constexpr optional() noexcept : m_empty{} {}
  constexpr optional(nullopt_t) noexcept : m_empty{} {}
  constexpr ~optional() { reset(); }

  template <class U = T> requires(is_constructible_v<T, U &&> && !is_optional_v<remove_cvref_t<U>> &&
                                  not_same<in_place_t, remove_cvref_t<U>>) constexpr optional(U &&u) {
    construct_in_place(forward<U>(u));
  }

  template <class... Args> requires is_constructible_v<T, Args &&...> constexpr explicit optional(in_place_t, Args &&...args) {
    construct_in_place(forward<Args>(args)...);
  }

  constexpr optional(const optional &rhs) requires(is_trivially_copyable_v<T>) = default;

  constexpr optional(const optional &rhs) requires(is_copy_constructible_v<T> && !is_trivially_copyable_v<T>) {
    if (rhs) {
      construct_in_place(*rhs);
    }
  }

  constexpr optional(optional &&rhs) noexcept {
    if (rhs) {
      construct_in_place(move(*rhs));
    }
  }

  constexpr optional &operator=(nullopt_t) noexcept { reset(); }

  constexpr optional &operator=(const optional &rhs) {
    if (this == &rhs)
      return *this;
    if (!rhs.has_value()) {
      reset();
    } else if (has_value()) {
      m_value = *rhs; // copy-assign
    } else {
      construct_in_place(*rhs);
    }
    return *this;
  }

  constexpr optional &operator=(optional &&rhs) noexcept {
    if (this == &rhs)
      return *this;
    if (!rhs.has_value()) {
      reset();
    } else if (has_value()) {
      m_value = move(*rhs); // move-assign
    } else {
      construct_in_place(move(*rhs)); // in-place move construct
    }
    return *this;
  }

  template <class U = T> requires(is_constructible_v<T, U> && !is_optional_v<remove_cvref_t<U>>) constexpr optional &operator=(U &&value) {
    if (has_value()) {
      m_value = forward<U>(value);
    } else {
      construct_in_place(forward<U>(value));
    }
    return *this;
  }

  template <class Self> constexpr decltype(auto) operator*(this Self && self) noexcept { return forward_like<Self>(self.m_value); }

  constexpr const T *operator->() const noexcept { return &m_value; }
  constexpr T *operator->() noexcept { return &m_value; }

  constexpr operator bool() const noexcept { return m_has_value; }
  constexpr bool has_value() const noexcept { return m_has_value; }

  template <class Self> constexpr decltype(auto) value(this Self &&self) {
    if (self.has_value())
      return forward_like<Self>(self.m_value);
    throw(bad_optional_access{});
  }

  template <class Self, class U> constexpr auto value_or(this Self &&self, U &&default_value) {
    return self.m_has_value ? forward_like<Self>(self.m_value) : static_cast<T>(forward<U>(default_value));
  }

  constexpr void reset() noexcept {
    if constexpr (!is_trivially_destructible_v<T>) {
      if (m_has_value)
        destroy_at(addressof(m_value));
    }
    m_has_value = false;
  }

  constexpr void swap(optional &rhs) noexcept {
    if (*this && rhs) {
      using aria::swap;
      swap(this->value(), rhs.value());
    } else if (*this && !rhs) {
      swap_helper(*this, rhs);
    } else if (!*this && rhs) {
      swap_helper(rhs, *this);
    }
  }

  template <class... Args> constexpr T &emplace(Args &&...args) {
    reset();
    construct_in_place(forward<Args>(args)...);
    return **this;
  }

  template <class Self, class F> constexpr auto and_then(this Self &&self, F &&f) {
    using R = remove_cvref_t<invoke_result_t<F, decltype(forward_like<Self>(self.m_value))>>;
    static_assert(is_optional_v<R>);
    if (self)
      return invoke(forward<F>(f), *forward<Self>(self));
    else
      return R{};
  }

  template <class Self, class F> constexpr auto or_else(this Self &&self, F &&f) {
    using R = remove_cvref_t<invoke_result_t<F>>;
    static_assert(same_as<optional<T>, R>);
    if (self)
      return forward<Self>(self);
    else
      return invoke(forward<F>(f));
  }

private:
  template <class... Args> constexpr void construct_in_place(Args &&...args) {
    construct_at(addressof(m_value), forward<Args>(args)...);
    m_has_value = true;
  }

  static void swap_helper(optional &has, optional &none) noexcept {
    none.construct_in_place(move(has.value()));
    has.reset();
  }

  struct empty_byte {};
  union {
    empty_byte m_empty;
    T m_value;
  };
  bool m_has_value = false;
};

template <class T, class... Args> constexpr optional<T> make_optional(Args &&...args) {
  return optional<T>(in_place, forward<Args>(args)...);
}

template <class T> void swap(optional<T> &a, optional<T> &b) noexcept { a.swap(b); }

} // namespace aria