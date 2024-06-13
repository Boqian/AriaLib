#pragma once
#include "exception.h"
#include "type_traits.h"
#include "unique_ptr.h"

namespace aria {

class bad_any_cast : public std::exception {
public:
  [[no_discard]] const char *what() const noexcept override { return "bad any cast"; }
};

class any {
public:
  constexpr any() noexcept = default;
  ~any() = default;

  any(const any &rhs) {
    if (rhs.has_value())
      m_ptr = rhs.m_ptr->clone();
  }

  template <class T>
    requires !is_same_v<remove_cvref_t<T>, any>
             any(T && value) : m_ptr(make_unique<storage<remove_cvref_t<T>>>(value)) {}

  constexpr bool has_value() const noexcept { return m_ptr; }

  template <class T> friend T any_cast(const any &x);

  constexpr void swap(any &rhs) noexcept { m_ptr.swap(rhs.m_ptr); }
  constexpr void reset() noexcept { m_ptr.reset(); }

private:
  struct storage_base {
    virtual ~storage_base() = default;
    virtual unique_ptr<storage_base> clone() const = 0;
  };

  template <class T> struct storage : storage_base {
    storage(const T &x) : value(x) {}
    unique_ptr<storage_base> clone() const override { return make_unique<storage<T>>(value); }

    T value;
  };

  unique_ptr<storage_base> m_ptr;
};

template <class T> T any_cast(const any &x) {
  auto p = dynamic_cast<any::storage<T> *>(x.m_ptr.get());
  if (!p)
    throw bad_any_cast();
  return p->value;
}

} // namespace aria