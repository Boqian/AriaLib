#include "ranges.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_ranges, begin_end) {
  int x[10]{};
  auto it = ranges::begin(x);
  static_assert(is_same_v<decltype(it), int *>);
  *it = 5;
  EXPECT_EQ(x[0], 5);

  auto it2 = ranges::end(x);
  static_assert(is_same_v<decltype(it2), int *>);
  EXPECT_EQ(x + 10, it2);
}