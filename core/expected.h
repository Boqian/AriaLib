#pragma once
#include "allocator.h"
#include "cassert"
#include "exception.h"
#include "functional.h"
#include "utility.h"

namespace aria {

//-----------------------bad_expected_access-----------------------

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
  explicit bad_expected_access(E err) : m_err(move(err)) {}
  template <class Self> decltype(auto) error(this Self &&self) noexcept { return forward<Self>(self).m_err; }

private:
  E m_err;
};

struct unexpect_t {
  explicit unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

//-----------------------unexpected-----------------------

template <class E> class unexpected;
template <class E> inline constexpr bool is_unexpected_v = is_specialization_of_v<E, unexpected>;

template <class E> class unexpected {
public:
  static_assert(is_object_v<E> && !is_array_v<E> && !is_const_v<E> && !same_as<E, in_place_t>, "invalid type of E.");

  template <class U = E>
  requires(!is_unexpected_v<remove_cvref_t<U>> && !same_as<remove_cvref_t<U>, in_place_t> && is_constructible_v<E, U>)
  constexpr explicit unexpected(U &&u) : m_err(forward<U>(u)) {}

  template <class... Args> requires(is_constructible_v<E, Args...>)
  constexpr explicit unexpected(in_place_t, Args &&...args) : m_err(forward<Args>(args)...) {}

  template <class Self> constexpr decltype(auto) error(this Self &&self) { return forward<Self>(self).m_err; }

  friend constexpr void swap(unexpected &x, unexpected &y);

private:
  E m_err;
};

template <class E> unexpected(E) -> unexpected<E>;

template <class E> requires(is_swappable_v<E>) constexpr void swap(unexpected<E> &x, unexpected<E> &y) {
  using aria::swap;
  swap(x.m_err, y.m_err);
}

template <class E> constexpr bool operator==(unexpected<E> &x, unexpected<E> &y) { return x.error() == y.error(); }

//-----------------------expected-----------------------

template <class T, class E> class expected;
template <class U> inline constexpr bool is_expected_v = is_specialization_of_v<U, expected>;

template <class T, class E> class expected {
  static constexpr bool is_trivially_copyable = is_trivially_copyable_v<T> && is_trivially_copyable_v<E>;
  static constexpr bool is_copy_constructible = is_copy_constructible_v<T> && is_copy_constructible_v<E>;
  static constexpr bool is_trivially_move_constructible = is_trivially_move_constructible_v<T> && is_trivially_move_constructible_v<E>;
  static constexpr bool is_move_constructible = is_move_constructible_v<T> && is_move_constructible_v<E>;

public:
  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;
  template <class U> using rebind = expected<U, error_type>;

  constexpr expected() requires is_default_constructible_v<T> : m_has_value(true), m_val() {}

  ~expected() { has_value() ? destroy_value() : destroy_error(); }

  constexpr expected(const expected &) requires is_trivially_copyable = default;
  constexpr expected(expected &&other) requires is_trivially_move_constructible = default;

  constexpr expected(const expected &other) requires(is_copy_constructible && !is_trivially_copyable) {
    if (other.has_value())
      construct_value(other.m_val);
    else
      construct_error(other.m_err);
  }

  constexpr expected(expected &&other) requires(is_move_constructible && !is_trivially_move_constructible) { swap(other); }

  constexpr expected &operator=(const expected &rhs) {
    if (this != &rhs)
      expected(rhs).swap(*this);
    return *this;
  }
  constexpr expected &operator=(expected &&rhs) noexcept {
    if (this != &rhs)
      expected(move(rhs)).swap(*this);
    return *this;
  }

  template <class U = T> requires(!is_same_v<remove_cvref_t<U>, in_place_t> && !is_expected_v<remove_cvref_t<U>> &&
                                  is_constructible_v<T, U>) constexpr explicit(!is_convertible_v<U, T>) expected(U &&v) {
    construct_value(forward<U>(v));
  }

  template <class G> requires(is_constructible_v<E, G>) constexpr explicit(!is_convertible_v<G, E>) expected(const unexpected<G> &e) {
    construct_error(e.error());
  }

  template <class G> requires(is_constructible_v<E, G>) constexpr explicit(!is_convertible_v<G, E>) expected(unexpected<G> &&e) {
    construct_error(move(e).error());
  }

  template <class... Args> requires(is_constructible_v<T, Args...>) constexpr explicit expected(in_place_t, Args &&...args) {
    construct_value(forward<Args...>(args)...);
  }

  template <class... Args> requires(is_constructible_v<E, Args...>) constexpr explicit expected(unexpect_t, Args &&...args) {
    construct_error(forward<Args...>(args)...);
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
    if (self.has_value())
      return forward_like<Self>(self.m_val);
    throw bad_expected_access(forward<Self>(self).m_err);
  }

  template <class Self> constexpr decltype(auto) error(this Self &&self) {
    assert(!self.has_value());
    return forward_like<Self>(self.m_err);
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

  template <class F, class Self> auto and_then(this Self &&self, F &&f) {
    using result_t = remove_cvref_t<invoke_result_t<F, decltype(*forward<Self>(self))>>;
    static_assert(is_expected_v<result_t> && same_as<E, typename result_t::error_type>);
    if (self.has_value()) {
      return invoke(forward<F>(f), *forward<Self>(self));
    } else {
      return result_t(unexpect, forward<Self>(self).error());
    }
  }

  template <class F, class Self> auto transform(this Self &&self, F &&f) {
    using U = remove_cvref_t<invoke_result_t<F, decltype(*forward<Self>(self))>>;
    if (self.has_value()) {
      return expected<U, E>(invoke(forward<F>(f), *forward<Self>(self)));
    } else {
      return expected<U, E>(unexpect, forward<Self>(self).error());
    }
  }

  template <class F, class Self> auto or_else(this Self &&self, F &&f) {
    using result_t = remove_cvref_t<invoke_result_t<F, decltype(forward<Self>(self).error())>>;
    static_assert(is_expected_v<result_t> && same_as<T, typename result_t::value_type>);
    if (self.has_value()) {
      return result_t(in_place, *forward<Self>(self));
    } else {
      return invoke(forward<F>(f), forward<Self>(self).error());
    }
  }

  template <class F, class Self> auto transform_error(this Self &&self, F &&f) {
    using G = remove_cvref_t<invoke_result_t<F, decltype(forward<Self>(self).error())>>;
    if (self.has_value()) {
      return expected<T, G>(in_place, *forward<Self>(self));
    } else {
      return expected<T, G>(unexpect, invoke(forward<F>(f), forward<Self>(self).error()));
    }
  }

  void swap(expected &b) noexcept {
    auto &a = *this;
    using aria::swap;
    if (a && b) {
      swap(*a, *b);
    } else if (!a && !b) {
      swap(a.m_err, b.m_err);
    } else if (!a && b) {
      b.swap(a);
    } else {
      auto temp_error = move(b.m_err);
      b.destroy_error();
      b.construct_value(*a);
      a.destroy_value();
      a.construct_error(move(temp_error));
    }
  }

private:
  template <class... Args> void construct_value(Args &&...args) {
    m_has_value = true;
    construct_at(addressof(m_val), forward<Args>(args)...);
  }

  template <class... Args> void construct_error(Args &&...args) {
    m_has_value = false;
    construct_at(addressof(m_err), forward<Args>(args)...);
  }

  void destroy_value() {
    if constexpr (!is_trivially_destructible_v<T>)
      destroy_at(addressof(m_val));
  }
  void destroy_error() {
    if constexpr (!is_trivially_destructible_v<E>)
      destroy_at(addressof(m_err));
  }

  union {
    T m_val;
    E m_err;
  };
  bool m_has_value;
};

template <class T, class E> void swap(expected<T, E> &a, expected<T, E> &b) noexcept { a.swap(b); }

} // namespace aria