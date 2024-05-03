#include "variant.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(is_same_v<int, best_overload_match<char, float, int, long, double>>);
static_assert(is_same_v<double, best_overload_match<float, int, double, bool>>);

TEST(test_variant, variant_storage) {
  using var = _variant_storage<int, double, char>;
  constexpr var x(integral_constant<size_t, 0>(), 1);
  static_assert(_variant_raw_get<0>(x) == 1);
  constexpr var y(integral_constant<size_t, 2>(), 'c');
  static_assert(_variant_raw_get<2>(y) == 'c');
}
