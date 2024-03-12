#pragma once
#include<utility> //std::forward

namespace aria {

/* similar to std::allocator
 * https://en.cppreference.com/w/cpp/memory/allocator
 */

template <class T, class... Args>
constexpr T* construct_at(T* p, Args&&... args) {
  return ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
}

template <class T>
constexpr void destroy_at(T* p) {
  p->~T();
}

template <class T>
struct allocator {
  using size_type = unsigned int;
  using value_type = T;
  using pointer = value_type*;

  allocator() noexcept = default;

  [[nodiscard]] pointer allocate(size_type n) {
    return reinterpret_cast<pointer>(::operator new(n * sizeof(T)));
  }

  void deallocate(T* p, size_type n) { ::operator delete(p, n); }
};


}  // namespace aria