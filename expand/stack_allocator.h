#pragma once
#include "cstddef.h"
#include "stdexcept.h"

namespace aria {

inline constexpr size_t DEFAULT_STACK_ALLOCATOR_SIZE = 16 * 1024; // 16kb

template <size_t N = DEFAULT_STACK_ALLOCATOR_SIZE> struct StackBuffer {
  constexpr size_t size() const { return N; }

  template <class T> T *allocate(size_t size) {
    const auto offset = get_offset<T>();
    if (pos + offset + size > N)
      throw bad_alloc();

    auto p = buffer + pos + offset;
    pos += offset + size * sizeof(T);
    return reinterpret_cast<T *>(p);
  }

  template <class T> constexpr size_t get_offset() const {
    auto diff = pos % alignof(T);
    return diff == 0 ? 0 : alignof(T) - diff;
  }

  alignas(alignof(max_align_t)) aria::byte buffer[N];
  size_t pos = 0;
};

template <class T, size_t N> class StackAllocator {
public:
  using value_type = T;
  using size_type = size_t;

  StackAllocator(StackBuffer<N> &buffer) : m_buffer(buffer) {}
  [[nodiscard]] constexpr T *allocate(size_type n) { return m_buffer.allocate<T>(n); }
  constexpr void deallocate(T *p, size_type n) {} // do nothing

private:
  StackBuffer<N> &m_buffer;
};

} // namespace aria