#pragma once
#include <utility> //std::forward
#include <memory> //std::default_delete
#include "concepts.h"

namespace aria {

template <class T, class Deleter = std::default_delete<T>>
class unique_ptr {
 public:
  using element_type = T;
  using pointer = T*;
  using deleter_type = Deleter;

  unique_ptr() = default;

  explicit unique_ptr(T* ptr) : m_ptr(ptr), m_deleter() {}

  unique_ptr(T* ptr, Deleter&& deleter)
      : m_ptr(ptr), m_deleter(std::forward<Deleter>(deleter)) {}

  unique_ptr(unique_ptr&& rhs) noexcept {
    swap(rhs);
    rhs.reset();
  }

  template <class U, class E>
    requires convertible_to<U*, T*>
  unique_ptr(unique_ptr<U, E>&& rhs)
      : m_ptr(rhs.release()), m_deleter(rhs.get_deleter()) {}


  ~unique_ptr() { reset(); }

  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const unique_ptr&) noexcept = delete;

  unique_ptr& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  unique_ptr& operator=(unique_ptr&& rhs) noexcept {
    swap(rhs);
    rhs.reset();
    return *this;
  }

  void reset() noexcept {
    if (m_ptr) {
      m_deleter(m_ptr);
      m_ptr = nullptr;
    }
  }

  T* get() const noexcept { return m_ptr; }

  const Deleter& get_deleter() const noexcept { return m_deleter; }
  Deleter& get_deleter() noexcept { return m_deleter; }

  operator bool() const noexcept { return m_ptr; }

  T* operator->() noexcept { return m_ptr; }
  const T* operator->() const noexcept { return m_ptr; }

  T& operator*() { return *m_ptr; }
  const T& operator*() const { return *m_ptr; }

  void swap(unique_ptr& rhs) noexcept {
    std::swap(m_ptr, rhs.m_ptr);
    std::swap(m_deleter, rhs.m_deleter);
  }

  T* release() noexcept {
    auto p = m_ptr;
    m_ptr = nullptr;
    return p;
  }

 private:
  T* m_ptr = nullptr;
  Deleter m_deleter;
};

template<class T, class... Args>
unique_ptr<T> make_unique(Args&&... args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace aria