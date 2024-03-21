#include "optional.h"
#include "gtest/gtest.h"

using namespace aria;



TEST(test_optional, basic) {
  optional<int> a;
  EXPECT_FALSE(a);
  constexpr optional<int> b;
  static_assert(!b);
}