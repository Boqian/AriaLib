#pragma once
#include "exception.h"
#include "utility.h"
#include <atomic>

namespace aria {

class bad_weak_ptr : public exception {
public:
  const char *what() const noexcept override { return "bad_weak_ptr"; }
};

template <class T> concept _can_scalar_delete = requires { delete declval<T *>(); };
template <class T> concept _can_array_delete = requires { delete[] declval<T *>(); };
template <class T> concept _can_delete = (is_array_v<T> && _can_array_delete<T>) || (!is_array_v<T> && _can_scalar_delete<T>);

template <class T, class U> struct _sp_convertible : is_convertible<T *, U *> {};
template <class T, class U> struct _sp_convertible<T, U[]> : is_convertible<T (*)[], U (*)[]> {};
template <class T, class U, size_t N> struct _sp_convertible<T, U[N]> : is_convertible<T (*)[N], U (*)[N]> {};
template <class T, class U> inline constexpr bool _sp_convertible_v = _sp_convertible<T, U>::value;

template <class T, class U> struct _sp_pointer_compatible : is_convertible<T *, U *> {};
template <class U, size_t N> struct _sp_pointer_compatible<U[N], U[]> : true_type {};
template <class U, size_t N> struct _sp_pointer_compatible<U[N], const U[]> : true_type {};
template <class U, size_t N> struct _sp_pointer_compatible<U[N], volatile U[]> : true_type {};
template <class U, size_t N> struct _sp_pointer_compatible<U[N], const volatile U[N]> : true_type {};
template <class T, class U> inline constexpr bool _sp_pointer_compatible_v = _sp_pointer_compatible<T, U>::value;

struct _ref_count_base {
  virtual ~_ref_count_base() = default;
  virtual void destory() const noexcept = 0;
  virtual void delete_this() noexcept = 0;
  virtual void *get_deleter(const type_info &) const noexcept { return nullptr; }

  void decrease_ref() noexcept {
    if (--m_uses == 0) {
      destory();
      if (m_weaks == 0)
        delete_this();
    }
  }

  void decrease_weak() noexcept {
    if (m_uses == 0 && --m_weaks == 0)
      delete this;
  }

  std::atomic<long> m_uses = 1;
  std::atomic<long> m_weaks = 0;
};

template <class T, bool is_array> struct _default_ref_count : _ref_count_base {
  explicit _default_ref_count(T *p) : m_ptr(p) {}
  _default_ref_count(const _default_ref_count &) = delete;
  void delete_this() noexcept override { delete this; }

  void destory() const noexcept override {
    if constexpr (!is_array)
      delete m_ptr;
    else
      delete[] m_ptr;
  }

  T *m_ptr;
};

template <class T, class Deleter> concept _valid_deleter = is_move_constructible_v<Deleter> && is_invocable_v<Deleter, T *>;

template <class T, class Deleter> requires _valid_deleter<T, Deleter> struct _ref_count_with_deleter : _ref_count_base {
  explicit _ref_count_with_deleter(T *p, Deleter &&d) : m_ptr(p), m_deleter(move(d)) {}
  void destory() const noexcept override { m_deleter(m_ptr); }
  void delete_this() noexcept override { delete this; }
  void *get_deleter(const type_info &info) const noexcept override {
    return typeid(Deleter) == info ? const_cast<Deleter *>(addressof(m_deleter)) : nullptr;
  }
  T *m_ptr;
  Deleter m_deleter;
};

template <class T> class weak_ptr;
template <class T> class enable_shared_from_this;

template <class T> class shared_ptr {
public:
  using element_type = remove_extent_t<T>;
  using weak_type = weak_ptr<T>;
  using pointer = element_type *;
  using reference = add_lvalue_reference_t<element_type>;
  static constexpr bool is_array = is_array_v<T>;

  shared_ptr() noexcept {}
  shared_ptr(nullptr_t) noexcept {}
  ~shared_ptr() {
    if (m_shared)
      m_shared->decrease_ref();
  }

  template <class U> requires(_sp_convertible_v<U, T> && _can_delete<U>)
  explicit shared_ptr(U *ptr) : m_ptr(ptr), m_shared(new _default_ref_count<U, is_array>(ptr)) {
    set_enable_from_this();
  }

  template <class U, class Deleter> requires(_sp_convertible_v<U, T> && _valid_deleter<U, Deleter>)
  shared_ptr(U *p, Deleter d) : m_ptr(p), m_shared(new _ref_count_with_deleter(p, move(d))) {
    set_enable_from_this();
  }

  // aliasing ctor
  template <class U> shared_ptr(const shared_ptr<U> &other, pointer p) noexcept {
    other.increase_ref();
    m_ptr = p;
    m_shared = other.m_shared;
  }
  // aliasing move ctor
  template <class U> shared_ptr(shared_ptr<U> &&other, pointer p) noexcept {
    m_ptr = p;
    m_shared = other.m_shared;
    other.m_ptr = nullptr;
    other.m_shared = nullptr;
  }

  template <class U> requires(_sp_pointer_compatible_v<U, T>) explicit shared_ptr(const weak_ptr<U> &other) {
    other.lock().swap(*this);
    if (!m_ptr)
      throw bad_weak_ptr();
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

  pointer get() const noexcept { return m_ptr; }
  explicit operator bool() const noexcept { return m_ptr; }
  long use_count() const noexcept { return m_shared ? m_shared->m_uses.load() : 0; }
  void reset() noexcept { shared_ptr().swap(*this); }
  reference operator*() const noexcept requires(!is_array && not_void<element_type>) { return *m_ptr; }
  pointer operator->() const noexcept requires(!is_array) { return m_ptr; }
  reference operator[](ptrdiff_t i) const requires(is_array) { return m_ptr[i]; }

  void swap(shared_ptr &rhs) noexcept {
    using aria::swap;
    swap(m_shared, rhs.m_shared);
    swap(m_ptr, rhs.m_ptr);
  }

private:
  friend class weak_ptr<T>; // weak_ptr.lock()
  template <class Deleter, class T> friend Deleter *get_deleter(const shared_ptr<T> &p) noexcept;
  template <class> friend class shared_ptr;

  void increase_ref() const {
    if (m_shared)
      ++m_shared->m_uses;
  }

  void set_enable_from_this() {
    if constexpr (is_base_of_v<enable_shared_from_this<T>, T>) {
      m_ptr->m_wptr = *this;
    }
  }

  pointer m_ptr{};
  _ref_count_base *m_shared{};
};

template <class T> class weak_ptr {
public:
  using element_type = remove_extent_t<T>;
  using pointer = element_type *;

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

  pointer m_ptr{};
  _ref_count_base *m_shared{};
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

template <class T> void swap(shared_ptr<T> &a, shared_ptr<T> &b) noexcept { a.swap(b); }
template <class T> void swap(weak_ptr<T> &a, weak_ptr<T> &b) noexcept { a.swap(b); }

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

template <class Deleter, class T> Deleter *get_deleter(const shared_ptr<T> &p) noexcept {
  if (p.m_shared)
    return static_cast<Deleter *>(p.m_shared->get_deleter(typeid(Deleter)));
  return nullptr;
}

} // namespace aria