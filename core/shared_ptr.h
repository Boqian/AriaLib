#pragma once
#include "utility.h"
#include <atomic>

namespace aria {

namespace detail {

struct shared_base {
  virtual ~shared_base() = default;
  virtual void del(void *ptr) const = 0;
  std::atomic<long> m_uses = 1;
  std::atomic<long> m_weaks = 0;

  void decrease_use(void *ptr) noexcept {
    if (--m_uses == 0) {
      del(ptr);
      if (m_weaks == 0)
        delete this;
    }
  }

  void decrease_weak() noexcept {
    if (m_uses == 0 && --m_weaks == 0)
      delete this;
  }
};

template <class T> struct default_shared : shared_base {
  void del(void *ptr) const override { delete static_cast<T *>(ptr); }
};
} // namespace detail

template <class T> class weak_ptr;

template <class T> class shared_ptr {
public:
  using element_type = T;

  shared_ptr() noexcept {}
  shared_ptr(std::nullptr_t) noexcept {}
  ~shared_ptr() {
    if (m_shared)
      m_shared->decrease_use(m_ptr);
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
  friend weak_ptr<T>;

  T *m_ptr{};
  detail::shared_base *m_shared{};
};

template <class T> class weak_ptr {
public:
  weak_ptr() noexcept = default;

  ~weak_ptr() noexcept {
    if (m_shared)
      m_shared->decrease_weak();
  }

  weak_ptr(const weak_ptr &r) noexcept : m_ptr(r.m_ptr), m_shared(r.m_shared) {
    if (m_shared) {
      m_shared->m_weaks++;
    }
  }

  template <class U>
    requires(convertible_to<U *, T *> and !is_same_v<U, T>)
  weak_ptr(const weak_ptr<U> &r) noexcept : m_ptr(r.m_ptr), m_shared(r.m_shared) {
    if (m_shared) {
      m_shared->m_weaks++;
    }
  }

  weak_ptr &operator=(const weak_ptr &rhs) noexcept {
    auto tmp = rhs;
    swap(tmp);
    return *this;
  }

  weak_ptr(weak_ptr &&rhs) noexcept { swap(rhs); }
  weak_ptr &operator=(weak_ptr &&rhs) {
    auto tmp = move(this);
    swap(tmp);
    return *this;
  }

  template <class U>
    requires convertible_to<U *, T *>
  weak_ptr(const shared_ptr<U> &r) noexcept : m_ptr(r.m_ptr), m_shared(r.m_shared) {
    if (m_shared) {
      m_shared->m_weaks++;
    }
  }

  long use_count() const noexcept { return m_shared ? m_shared->m_uses.load() : 0; }
  bool expired() const noexcept { return use_count() == 0; }

  shared_ptr<T> lock() const noexcept {
    if (!m_shared)
      return {};

    for (auto use_count = m_shared->m_uses.load(); use_count != 0; use_count = m_shared->m_uses.load()) {
      if (m_shared->m_uses.compare_exchange_strong(use_count, use_count + 1)) {
        shared_ptr<T> p;
        p.m_ptr = m_ptr;
        p.m_shared = m_shared;
        return p;
      }
    }
    return {};
  }

  void reset() noexcept { weak_ptr().swap(*this); }

  void swap(weak_ptr &rhs) noexcept {
    aria::swap(m_shared, rhs.m_shared);
    aria::swap(m_ptr, rhs.m_ptr);
  }

private:
  T *m_ptr{};
  detail::shared_base *m_shared{};
};

template <class T> void swap(shared_ptr<T> &a, shared_ptr<T> &b) { a.swap(b); }
template <class T> void swap(weak_ptr<T> &a, weak_ptr<T> &b) { a.swap(b); }

template <class T, class... Args> shared_ptr<T> make_shared(Args &&...args) { return shared_ptr<T>(new T(forward<Args>(args)...)); }

} // namespace aria