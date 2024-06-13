#pragma once
#include "exception.h"
#include "unique_ptr.h"

namespace aria {

class bad_any_cast : public std::exception {
public:
  [[no_discard]] const char *what() const noexcept override { return "bad any cast"; }
};

class any {
public:
  constexpr any() noexcept = default;

  template <class T> any(T &&value) : m_ptr(make_unique<storage<remove_cvref_t<T>>>(value)) {}

  constexpr bool has_value() const noexcept { return m_ptr; }

  template <class T> friend T any_cast(const any &x);

private:
  struct storage_base {
    virtual ~storage_base() = default;
  };

  template <class T> struct storage : storage_base {
    storage(T &x) : value(x) {}
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