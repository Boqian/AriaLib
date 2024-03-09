#pragma once
#include <atomic>
#include <functional>

#include "concepts.h"

namespace aria {

namespace details {

class shared {
 public:
   template <class Deleter>
   shared(Deleter&& deleter)
       : m_deleter(std::forward<Deleter>(deleter)), m_counter(1) {}

  template <class Y>
  static shared* create(Y* ptr) {
     return shared([ptr]() { delete ptr; });
    return nullptr;
  }

 private:
   std::atomic<int> m_counter;
   std::function<void()> m_deleter;
};
}  // namespace details

template <class T>
class shared_ptr {
 public:
  using element_type = T;

  shared_ptr() noexcept {}
  shared_ptr(std::nullptr_t) noexcept {}

  template <class Y>
    requires convertible_to<Y*, T*>
  explicit shared_ptr(Y* ptr)
      : m_ptr(ptr), m_shared(details::shared::create(ptr)) {}

  operator bool() const noexcept { return m_ptr; }

 private:
  T* m_ptr = nullptr;
  details::shared* m_shared = nullptr;
};

}  // namespace aria