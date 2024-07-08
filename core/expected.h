#pragma once
#include "cassert"
#include "exception.h"
#include "utility.h"

namespace aria {

template <class E> class bad_expected_access;

template <> class bad_expected_access<void> : public exception {
public:
  [[nodiscard]] const char *what() const noexcept override { return "Bad expected access"; }

protected:
  bad_expected_access() = default;
  bad_expected_access(const bad_expected_access &) = default;
  bad_expected_access(bad_expected_access &&) = default;
  bad_expected_access &operator=(const bad_expected_access &) = default;
  bad_expected_access &operator=(bad_expected_access &&) = default;
};

template <class E> class bad_expected_access : public bad_expected_access<void> {
public:
  explicit bad_expected_access(E unex) : unexpected(move(unex)) {}
  template <class Self> decltype(auto) error(this Self &&self) noexcept { return forward<Self>(self).unexpected; }

private:
  E unexpected;
};

struct unexpect_t {
  explicit unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

template <class T, class E> class expected {
public:
  using value_type = T;
  using error_type = E;
  template <class U> using rebind = expected<U, error_type>;

  constexpr expected() requires is_default_constructible_v<T> : m_has_value(true), m_val() {}

  ~expected() {
    if constexpr (!is_trivially_destructible_v<T>) {
      if (has_value())
        destroy_at(addressof(m_val));
    }
    if constexpr (!is_trivially_destructible_v<E>) {
      if (!has_value()) {
        destroy_at(addressof(m_err));
      }
    }
  }

  constexpr bool has_value() const noexcept { return m_has_value; }
  constexpr explicit operator bool() const noexcept { return m_has_value; }
  constexpr const T *operator->() const noexcept { return addressof(m_val); }
  constexpr T *operator->() noexcept { return addressof(m_val); }
  constexpr const T &operator*() const & noexcept { return m_val; }
  constexpr T &operator*() & noexcept { return m_val; }
  constexpr const T &&operator*() const && noexcept { return move(m_val); }
  constexpr T &&operator*() && noexcept { return move(m_val); }

  template <class Self> constexpr decltype(auto) value(this Self &&self) {
    if (has_value())
      return forward<Self>(self).m_val;
    throw bad_expected_access(forward<Self>(self).m_err);
  }

  template <class Self> constexpr decltype(auto) error(this Self &&self) {
    assert(!has_value());
    return forward<Self>(self).m_err;
  }

  template <class U> requires is_copy_constructible_v<T> && is_convertible_v<U, T> constexpr T value_or(U &&default_value) const & {
    return has_value() ? m_val : static_cast<T>(forward<U>(default_value));
  }
  template <class U> requires is_move_constructible_v<T> && is_convertible_v<U, T> constexpr T value_or(U &&default_value) && {
    return has_value() ? move(m_val) : static_cast<T>(forward<U>(default_value));
  }

  template <class G = E> requires is_copy_constructible_v<E> && is_convertible_v<G, E> constexpr E error_or(G &&default_value) const & {
    return has_value() ? static_cast<E>(forward<G>(default_value)) : m_err;
  }

  template <class G = E> requires is_move_constructible_v<E> && is_convertible_v<G, E> constexpr E error_or(G &&default_value) && {
    return has_value() ? static_cast<E>(forward<G>(default_value)) : move(m_err);
  }

private:
  union {
    T m_val;
    E m_err;
  };
  bool m_has_value;
};

} // namespace aria