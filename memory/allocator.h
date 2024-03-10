#pragma once
namespace aria {

/* similar to std::allocator
 * https://en.cppreference.com/w/cpp/memory/allocator
 */

template <class T>
struct allocator {
  using size_type = unsigned int;
  using value_type = T;
  using pointer = value_type*;

  allocator() noexcept = default;

  [[nodiscard]] pointer allocate(size_type n) {
    return reinterpret_cast<pointer>(::operator new(n * sizeof(T)));
  }

  void deallocate(T* p, size_type n) { ::operator delete(p); }
};


}  // namespace aria