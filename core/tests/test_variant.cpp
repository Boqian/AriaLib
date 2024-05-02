#include "variant.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_variant, variant_storage) {
  using var = _variant_storage<int, double, char>;
  constexpr var x(integral_constant<size_t, 0>(), 1);
  static_assert(_variant_raw_get<0>(x) == 1);
  constexpr var y(integral_constant<size_t, 2>(), 'c');
  static_assert(_variant_raw_get<2>(y) == 'c');
}