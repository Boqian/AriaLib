#pragma once
#include "utility.h"

namespace aria {

template <class T> struct default_delete {
  constexpr default_delete() noexcept = default;
  template <class U> requires convertible_to<U *, T *> constexpr default_delete(const default_delete<U> &) noexcept {}

  void operator()(T *ptr) const noexcept {
    static_assert(0 < sizeof(T), "can't delete incomplete type");
    delete ptr;
  }
};

template <class T> struct default_delete<T[]> {
  constexpr default_delete() noexcept = default;
  template <class U> requires is_convertible_v<U (*)[], T (*)[]> constexpr default_delete(const default_delete<U[]> &) noexcept {}

  template <class U> requires is_convertible_v<U (*)[], T (*)[]> constexpr void operator()(U *ptr) const noexcept {
    static_assert(0 < sizeof(U), "can't delete an incomplete type");
    delete[] ptr;
  }
};

template <class T, class Deleter = default_delete<T>> class unique_ptr {
public:
  using element_type = remove_extent_t<T>;
  using pointer = element_type *;
  using deleter_type = Deleter;
  static constexpr bool is_array = is_array_v<T>;

  constexpr unique_ptr() = default;

  explicit constexpr unique_ptr(pointer ptr) : m_ptr(ptr), m_deleter() {}

  constexpr unique_ptr(pointer ptr, Deleter &&deleter) : m_ptr(ptr), m_deleter(forward<Deleter>(deleter)) {}

  template <class U, class E> requires(convertible_to<U *, pointer> && !is_array)
  constexpr unique_ptr(unique_ptr<U, E> &&rhs) : m_ptr(rhs.release()), m_deleter(rhs.get_deleter()) {}

  constexpr ~unique_ptr() {
    if (m_ptr) {
      m_deleter(m_ptr);
      m_ptr = nullptr;
    }
  }

  constexpr unique_ptr(const unique_ptr &) = delete;
  constexpr unique_ptr &operator=(const unique_ptr &) noexcept = delete;
  constexpr unique_ptr &operator=(nullptr_t) noexcept {
    reset();
    return *this;
  }
  constexpr unique_ptr(unique_ptr &&rhs) noexcept { swap(rhs); }
  constexpr unique_ptr &operator=(unique_ptr &&rhs) noexcept {
    unique_ptr(move(rhs)).swap(*this);
    return *this;
  }

  constexpr void reset() noexcept { unique_ptr().swap(*this); }

  constexpr pointer get() const noexcept { return m_ptr; }

  constexpr const Deleter &get_deleter() const noexcept { return m_deleter; }
  constexpr Deleter &get_deleter() noexcept { return m_deleter; }

  constexpr operator bool() const noexcept { return m_ptr; }

  constexpr pointer operator->() const noexcept requires(!is_array) { return m_ptr; }
  constexpr element_type &operator*() const requires(!is_array) { return *m_ptr; }
  constexpr element_type &operator[](size_t i) const requires(is_array) { return m_ptr[i]; }

  constexpr void swap(unique_ptr &rhs) noexcept {
    using aria::swap;
    swap(m_ptr, rhs.m_ptr);
    swap(m_deleter, rhs.m_deleter);
  }

  constexpr T *release() noexcept {
    auto p = m_ptr;
    m_ptr = nullptr;
    return p;
  }

private:
  pointer m_ptr = nullptr;
  Deleter m_deleter;
};

template <class T, class D> constexpr void swap(unique_ptr<T, D> &a, unique_ptr<T, D> &b) { a.swap(b); }

template <class T, class... Args> requires(!is_array_v<T>) unique_ptr<T> make_unique(Args &&...args) {
  return unique_ptr<T>(new T(forward<Args>(args)...));
}

template <class T> requires is_array_v<T> constexpr unique_ptr<T> make_unique(size_t size) {
  return unique_ptr<T>(new remove_extent_t<T>[size]);
}

} // namespace aria