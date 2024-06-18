#include "span.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_span, constructor) {
  array<int, 3> v = {1, 2, 3};
  span sp(v);
  EXPECT_EQ(sp.front(), 1);
  EXPECT_EQ(sp.back(), 3);
  EXPECT_EQ(sp.size(), 3);
}