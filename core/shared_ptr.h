#pragma once
#include "utility.h"
#include <atomic>

namespace aria {

namespace detail {

struct shared_base {
  virtual ~shared_base() = default;
  virtual void del(void *ptr) const = 0;
  std::atomic<long> m_uses = 1;
  std::atomic<long> m_weaks = 1;
};

template <class T> struct default_shared : shared_base {
  void del(void *ptr) const override { delete static_cast<T *>(ptr); }
};
} // namespace detail

template <class T> class shared_ptr {
public:
  using element_type = T;

  shared_ptr() noexcept {}
  shared_ptr(std::nullptr_t) noexcept {}
  ~shared_ptr() {
    if (m_shared) {
      if (--m_shared->m_uses == 0) {
        m_shared->del(m_ptr);
        delete m_shared;
      }
    }
  }

  template <class Y>
    requires convertible_to<Y *, T *>
  explicit shared_ptr(Y *ptr) : m_ptr(ptr), m_shared(new detail::default_shared<Y>()) {}

  shared_ptr(shared_ptr &&rhs) noexcept { swap(rhs); }

  shared_ptr(const shared_ptr &rhs) : m_ptr(rhs.m_ptr), m_shared(rhs.m_shared) {
    if (m_shared)
      m_shared->m_uses++;
  }

  shared_ptr &operator=(const shared_ptr &rhs) noexcept {
    if (this != &rhs) {
      auto tmp = rhs;
      swap(tmp);
    }
    return *this;
  }

  shared_ptr &operator=(shared_ptr &&rhs) noexcept {
    auto tmp = move(rhs);
    swap(tmp);
    return *this;
  }

  T *get() { return m_ptr; }
  const T *get() const { return m_ptr; }

  const add_lvalue_reference_t<T> operator*() const
    requires !is_void_v<T>
  {
    return *m_ptr;
  }
  const T *operator->() const { return m_ptr; }

  operator bool() const noexcept { return m_ptr; }

  long use_count() const noexcept { return m_shared ? m_shared->m_uses.load() : 0; }
  bool unique() const noexcept { return use_count() == 1; }

  void reset() noexcept { shared_ptr().swap(*this); }

  void swap(shared_ptr &rhs) noexcept {
    aria::swap(m_shared, rhs.m_shared);
    aria::swap(m_ptr, rhs.m_ptr);
  }

private:
  T *m_ptr{};
  detail::shared_base *m_shared{};
};

template <class T> void swap(shared_ptr<T> &a, shared_ptr<T> &b) { a.swap(b); }

template <class T, class... Args> shared_ptr<T> make_shared(Args &&...args) { return shared_ptr<T>(new T(forward<Args>(args)...)); }

} // namespace aria