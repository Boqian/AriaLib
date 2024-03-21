#pragma once
#include <atomic>
#include "functional.h"
#include "utility.h"
#include "concepts.h"

namespace aria {

namespace details {

class shared {
 public:
   template <class Deleter>
   shared(Deleter&& deleter)
      : m_deleter(forward<Deleter>(deleter)), m_uses(1), m_weaks(0) {}

   ~shared() { m_deleter(); }

  template <class Y>
   static shared* create(Y* ptr) {
     return new shared([ptr]() { delete ptr; });
   }

   std::atomic<long> m_uses;
   std::atomic<long> m_weaks;
   function<void()> m_deleter;
};
}  // namespace details

template <class T>
class shared_ptr {
 public:
  using element_type = T;

  shared_ptr() noexcept {}
  shared_ptr(std::nullptr_t) noexcept {}
  ~shared_ptr() { reset(); }

  template <class Y>
    requires convertible_to<Y*, T*>
  explicit shared_ptr(Y* ptr)
      : m_ptr(ptr), m_shared(details::shared::create(ptr)) {}

  shared_ptr(shared_ptr&& rhs) { 
    swap(rhs);
    shared_ptr().swap(rhs);
  }

  shared_ptr(const shared_ptr& rhs) : m_ptr(rhs.m_ptr), m_shared(rhs.m_shared) {
    if (m_shared) m_shared->m_uses++;
  }

  shared_ptr& operator=(const shared_ptr& rhs) noexcept {
    if (this == &rhs)
      return *this;
    reset();
    m_ptr = rhs.m_ptr;
    m_shared = rhs.m_shared;
    if (m_shared) m_shared->m_uses++;
    return *this;
  }

  shared_ptr& operator=(shared_ptr&& rhs) noexcept {
    if (this == &rhs)
      return *this;
    swap(rhs);
    shared_ptr().swap(rhs);
    return *this;
  }

  T* get() { return m_ptr; }
  const T* get() const { return m_ptr; }

  const add_lvalue_reference_t<T> operator*() const
    requires !is_void_v<T>
  {
    return *m_ptr;
  }
  const T* operator->() const { return m_ptr; }

  operator bool() const noexcept { return m_ptr; }

  long use_count() const noexcept { return m_shared ? m_shared->m_uses.load() : 0; }
  bool unique() const noexcept { return use_count() == 1; }

  void reset() noexcept{
    if (m_shared) {
      if (--m_shared->m_uses == 0) {
        delete m_shared;  
      }
      m_shared = nullptr;    
      m_ptr = nullptr;
    }
  }

  void swap(shared_ptr& rhs) noexcept {
    aria::swap(m_shared, rhs.m_shared);
    aria::swap(m_ptr, rhs.m_ptr);
  }

 private:
  T* m_ptr = nullptr;
  details::shared* m_shared = nullptr;
};

template<class T, class... Args> 
shared_ptr<T> make_shared(Args&&... args) {
  return shared_ptr<T>(new T(forward<Args>(args)...));
}

}  // namespace aria