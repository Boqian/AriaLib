#pragma once

namespace aria {
namespace meta {
// compile time gcd
template <unsigned int a, unsigned int b>
constexpr int gcd() {
  if constexpr (a < b) {
    return gcd<b, a>();
  } else if constexpr (b == 0) {
    return a;
  } else {
    return gcd<b, a % b>();
  }
}

static_assert(gcd<14, 6>() == 2, "");

}  // namespace meta
}  // namespace aria