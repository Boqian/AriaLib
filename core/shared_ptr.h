#pragma once
#include "unique_ptr.h"
#include "utility.h"
#include <atomic>

namespace aria {

namespace detail {

struct IDeleter {
  IDeleter() = default;
  virtual ~IDeleter() = default;
  virtual void operator()(void *) const = 0;
};

template <class T> struct DefaultDeleter : IDeleter {
  void operator()(void *ptr) const override { delete static_cast<T *>(ptr); }
};

template <class F, class T> struct CustomDeleter : IDeleter {
  CustomDeleter(F &&af) : f(aria::forward<F>(af)) {}
  void operator()(void *ptr) const override { f(static_cast<T *>(ptr)); }
  F f;
};

class shared {
public:
  shared(unique_ptr<IDeleter> deleter) : m_deleter(move(deleter)) {}
  template <class T> static shared *create() { return new shared(make_unique<DefaultDeleter<T>>()); }
  std::atomic<long> m_uses = 1;
  std::atomic<long> m_weaks = 1;
  unique_ptr<IDeleter> m_deleter;
};
} // namespace detail

template <class T> class shared_ptr {
public:
  using element_type = T;

  shared_ptr() noexcept {}
  shared_ptr(std::nullptr_t) noexcept {}
  ~shared_ptr() { reset(); }

  template <class Y>
    requires convertible_to<Y *, T *>
  explicit shared_ptr(Y *ptr) : m_ptr(ptr), m_shared(detail::shared::create<Y>()) {}

  shared_ptr(shared_ptr &&rhs) {
    swap(rhs);
    shared_ptr().swap(rhs);
  }

  shared_ptr(const shared_ptr &rhs) : m_ptr(rhs.m_ptr), m_shared(rhs.m_shared) {
    if (m_shared)
      m_shared->m_uses++;
  }

  shared_ptr &operator=(const shared_ptr &rhs) noexcept {
    if (this == &rhs)
      return *this;
    reset();
    m_ptr = rhs.m_ptr;
    m_shared = rhs.m_shared;
    if (m_shared)
      m_shared->m_uses++;
    return *this;
  }

  shared_ptr &operator=(shared_ptr &&rhs) noexcept {
    if (this == &rhs)
      return *this;
    swap(rhs);
    shared_ptr().swap(rhs);
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

  void reset() noexcept {
    if (m_shared) {
      if (--m_shared->m_uses == 0) {
        m_shared->m_deleter->operator()(m_ptr);
        delete m_shared;
      }
      m_shared = nullptr;
      m_ptr = nullptr;
    }
  }

  void swap(shared_ptr &rhs) noexcept {
    aria::swap(m_shared, rhs.m_shared);
    aria::swap(m_ptr, rhs.m_ptr);
  }

private:
  T *m_ptr{};
  detail::shared *m_shared{};
};

template <class T, class... Args> shared_ptr<T> make_shared(Args &&...args) { return shared_ptr<T>(new T(forward<Args>(args)...)); }

} // namespace aria