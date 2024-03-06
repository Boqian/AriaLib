#pragma once
namespace aria {

/* similar to std::allocator
 * https://en.cppreference.com/w/cpp/memory/allocator
 */

template <class T>
struct allocator {
  [[nodiscard]] T* allocate(std::size_t n) {}

  void deallocate(T* p, std::size_t n) {}

};


}  // namespace aria