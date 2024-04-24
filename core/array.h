#pragma once
#include "utility.h"

namespace aria {

template <class T, size_t N> class array {
public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;

  reference operator[](size_t i) { return arr[i]; }
  const_reference operator[](size_t i) const { return arr[i]; }

private:
  T arr[N] = {};
};

} // namespace aria