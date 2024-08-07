#pragma once
#include "memory.h"
#include "stdexcept.h"
#include "utility.h"
#include <typeinfo>

namespace aria {

class bad_any_cast : public exception {
public:
  [[no_discard]] const char *what() const noexcept override { return "bad any cast"; }
};

class any {
public:
  any() noexcept = default;

  any(const any &rhs) {
    if (rhs.m_ptr)
      m_ptr = rhs.m_ptr->clone();
  }

  template <class T, class... Args>
  explicit any(in_place_type_t<T>, Args &&...args) : m_ptr(make_unique<storage<T>>(forward<Args>(args)...)) {}

  template <class T, class U = remove_cvref_t<T>> requires(not_same<U, any> && !is_in_place_type_v<U>)
  any(T &&value) : m_ptr(make_unique<storage<U>>(value)) {}

  template <class T> requires not_same<remove_cvref_t<T>, any> any &operator=(T &&value) {
    any tmp = forward<T>(value);
    swap(tmp);
    return *this;
  }

  any(any &&rhs) noexcept { swap(rhs); }

  any &operator=(const any &rhs) {
    if (this != &rhs) {
      auto tmp = rhs;
      swap(tmp);
    }
    return *this;
  }

  any &operator=(any &&rhs) noexcept {
    if (this != &rhs) {
      auto tmp = move(rhs);
      swap(tmp);
    }
    return *this;
  }

  constexpr bool has_value() const noexcept { return m_ptr; }
  constexpr void swap(any &rhs) noexcept { m_ptr.swap(rhs.m_ptr); }
  constexpr void reset() noexcept { m_ptr.reset(); }
  const std::type_info &type() const noexcept { return m_ptr ? m_ptr->type() : typeid(void); }

  template <class T> friend T any_cast(const any &x);
  template <class T> friend T any_cast(any &&x);
  template <class T> friend const T *any_cast(const any *x) noexcept;
  template <class T> friend T *any_cast(any *x) noexcept;

private:
  struct storage_base {
    virtual ~storage_base() = default;
    virtual unique_ptr<storage_base> clone() const = 0;
    virtual const std::type_info &type() const = 0;
  };

  template <class T> struct storage : storage_base {
    storage(const T &x) : value(x) {}
    template <class... Args> storage(Args... args) : value(forward<Args>(args)...) {}

    unique_ptr<storage_base> clone() const override { return make_unique<storage<T>>(value); }
    const std::type_info &type() const override { return typeid(T); }
    T value;
  };

  unique_ptr<storage_base> m_ptr;
};

void swap(any &lhs, any &rhs) noexcept { lhs.swap(rhs); }

template <class T, class... Args> any make_any(Args &&...args) { return any(in_place_type<T>, forward<Args>(args)...); }

template <class T> T any_cast(const any &x) {
  using U = remove_cvref_t<T>;
  auto p = dynamic_cast<any::storage<U> *>(x.m_ptr.get());
  if (!p)
    throw bad_any_cast();
  return static_cast<T>(p->value);
}

template <class T> T any_cast(any &&x) {
  using U = remove_cvref_t<T>;
  auto p = dynamic_cast<any::storage<U> *>(x.m_ptr.get());
  if (!p)
    throw bad_any_cast();
  return static_cast<T>(move(p->value));
}

template <class T> const T *any_cast(const any *x) noexcept {
  if (!x || !x->has_value())
    return nullptr;
  using U = remove_cvref_t<T>;
  auto p = dynamic_cast<any::storage<U> *>(x->m_ptr.get());
  return p ? addressof(p->value) : nullptr;
}

template <class T> T *any_cast(any *x) noexcept {
  if (!x || !x->has_value())
    return nullptr;
  using U = remove_cvref_t<T>;
  auto p = dynamic_cast<any::storage<U> *>(x->m_ptr.get());
  return p ? addressof(p->value) : nullptr;
}

} // namespace aria