#pragma once

#include "allocator.h"

namespace aria {

template <class T, class Allocator = allocator<T>>
class vector {
 public:
  using size_type = unsigned int;
  using value_type = T;
  using pointer = value_type*;

  vector() noexcept = default;

 private:
  Allocator m_allocator;
  pointer m_ptr = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;

}; 

}  // namespace aria