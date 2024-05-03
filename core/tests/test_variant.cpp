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

TEST(test_variant, basic) {
  using var = variant<int, double, char>;
  {
    constexpr var v(123);
    static_assert(v.index() == 0);
    static_assert(holds_alternative<int>(v));
    EXPECT_EQ(v.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(v));
  }
  {
    constexpr var v(123.0);
    static_assert(v.index() == 1);
    static_assert(holds_alternative<double>(v));
    EXPECT_EQ(v.index(), 1);
    EXPECT_TRUE(holds_alternative<double>(v));
  }
  {
    constexpr var v('e');
    static_assert(v.index() == 2);
    static_assert(holds_alternative<char>(v));
    EXPECT_EQ(v.index(), 2);
    EXPECT_TRUE(holds_alternative<char>(v));
  }
  {
    constexpr var v;
    static_assert(v.index() == variant_npos);
  }
  static_assert(is_same_v<variant_alternative_t<0, var>, int>);
  static_assert(is_same_v<variant_alternative_t<1, var>, double>);
  static_assert(is_same_v<variant_alternative_t<2, var>, char>);
}
