#pragma once
#include "utility.h"
#include <atomic>

namespace aria {

struct _shared_base {
  virtual ~_shared_base() = default;
  virtual void destory() const = 0;

  void decrease_ref() noexcept {
    if (--m_uses == 0) {
      destory();
      if (m_weaks == 0)
        delete this;
    }
  }

  void decrease_weak() noexcept {
    if (m_uses == 0 && --m_weaks == 0)
      delete this;
  }

  std::atomic<long> m_uses = 1;
  std::atomic<long> m_weaks = 0;
};

template <class T> struct _default_shared : _shared_base {
  explicit _default_shared(T *p) : m_ptr(p) {}
  void destory() const override { delete m_ptr; }
  T *m_ptr;
};

template <class T> class weak_ptr;
template <class T> class enable_shared_from_this;

template <class T> class shared_ptr {
public:
  using element_type = T;

  shared_ptr() noexcept {}
  shared_ptr(nullptr_t) noexcept {}
  ~shared_ptr() {
    if (m_shared)
      m_shared->decrease_ref();
  }

  template <class Y> requires convertible_to<Y *, T *> explicit shared_ptr(Y *ptr) : m_ptr(ptr), m_shared(new _default_shared<Y>(ptr)) {
    if constexpr (is_base_of_v<enable_shared_from_this<T>, T>) {
      m_ptr->m_wptr = *this;
    }
  }

  // aliasing ctor
  template <class U> shared_ptr(const shared_ptr<U> &other, element_type *p) noexcept {
    other.increase_ref();
    m_ptr = p;
    m_shared = other.m_shared;
  }
  // aliasing move ctor
  template <class U> shared_ptr(shared_ptr<U> &other, element_type *p) noexcept {
    m_ptr = p;
    m_shared = other.m_shared;
    other.m_ptr = nullptr;
    other.m_shared = nullptr;
  }

  shared_ptr(shared_ptr &&rhs) noexcept { swap(rhs); }

  shared_ptr(const shared_ptr &rhs) : m_ptr(rhs.m_ptr), m_shared(rhs.m_shared) { increase_ref(); }

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

  element_type *get() const { return m_ptr; }
  add_lvalue_reference_t<T> operator*() { return *m_ptr; }
  const add_lvalue_reference_t<T> operator*() const { return *m_ptr; }
  T *operator->() { return m_ptr; }
  const T *operator->() const { return m_ptr; }
  operator bool() const noexcept { return m_ptr; }
  long use_count() const noexcept { return m_shared ? m_shared->m_uses.load() : 0; }
  void reset() noexcept { shared_ptr().swap(*this); }

  void swap(shared_ptr &rhs) noexcept {
    using aria::swap;
    swap(m_shared, rhs.m_shared);
    swap(m_ptr, rhs.m_ptr);
  }

private:
  friend class weak_ptr<T>; // weak_ptr.lock()
  template <class> friend class shared_ptr;

  void increase_ref() const {
    if (m_shared)
      ++m_shared->m_uses;
  }

  T *m_ptr{};
  _shared_base *m_shared{};
};

template <class T> class weak_ptr {
public:
  weak_ptr() noexcept = default;

  ~weak_ptr() noexcept {
    if (m_shared)
      m_shared->decrease_weak();
  }

  weak_ptr(const weak_ptr &r) noexcept : m_ptr(r.m_ptr), m_shared(r.m_shared) { increase_weak(); }

  template <class U> requires(convertible_to<U *, T *> and !is_same_v<U, T>)
  weak_ptr(const weak_ptr<U> &r) noexcept : m_ptr(r.m_ptr), m_shared(r.m_shared) {
    increase_weak();
  }

  weak_ptr &operator=(const weak_ptr &rhs) noexcept {
    auto tmp = rhs;
    swap(tmp);
    return *this;
  }

  template <class Y> weak_ptr &operator=(const shared_ptr<Y> &r) noexcept {
    weak_ptr<T>(r).swap(*this);
    return *this;
  }

  weak_ptr(weak_ptr &&rhs) noexcept { swap(rhs); }
  weak_ptr &operator=(weak_ptr &&rhs) {
    auto tmp = move(this);
    swap(tmp);
    return *this;
  }

  template <class U> requires convertible_to<U *, T *> weak_ptr(const shared_ptr<U> &r) noexcept : m_ptr(r.m_ptr), m_shared(r.m_shared) {
    increase_weak();
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
    using aria::swap;
    swap(m_shared, rhs.m_shared);
    swap(m_ptr, rhs.m_ptr);
  }

private:
  void increase_weak() const {
    if (m_shared)
      m_shared->m_weaks++;
  }

  T *m_ptr{};
  _shared_base *m_shared{};
};

template <class T> class enable_shared_from_this {
public:
  shared_ptr<T> shared_from_this() const { return m_wptr.lock(); }
  weak_ptr<T> weak_from_this() const noexcept { return m_wptr; }

protected:
  constexpr enable_shared_from_this() noexcept {}

private:
  template <class U> friend class shared_ptr;
  weak_ptr<T> m_wptr;
};

template <class T> void swap(shared_ptr<T> &a, shared_ptr<T> &b) { a.swap(b); }
template <class T> void swap(weak_ptr<T> &a, weak_ptr<T> &b) { a.swap(b); }

template <class T, class... Args> shared_ptr<T> make_shared(Args &&...args) { return shared_ptr<T>(new T(forward<Args>(args)...)); }

template <class T, class U> shared_ptr<T> static_pointer_cast(const shared_ptr<U> &r) noexcept {
  auto p = static_cast<typename shared_ptr<T>::element_type *>(r.get());
  return shared_ptr<T>{r, p};
}

template <class T, class U> shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U> &r) noexcept {
  if (auto p = dynamic_cast<typename shared_ptr<T>::element_type *>(r.get()))
    return shared_ptr<T>{r, p};
  else
    return {};
}

template <class T, class U> shared_ptr<T> const_pointer_cast(const shared_ptr<U> &r) noexcept {
  auto p = const_cast<typename shared_ptr<T>::element_type *>(r.get());
  return shared_ptr<T>{r, p};
}

template <class T, class U> shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U> &r) noexcept {
  auto p = reinterpret_cast<typename shared_ptr<T>::element_type *>(r.get());
  return shared_ptr<T>{r, p};
}

} // namespace aria