#include "utility.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_utility, pair) {
  {
    pair<int, int> p(5, 6);
    EXPECT_EQ(p.first, 5);
    EXPECT_EQ(p.second, 6);
  }

}
