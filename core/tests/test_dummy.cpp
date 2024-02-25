#include "../dummy.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_dummy, add) {
  EXPECT_EQ(3, core::add(1, 2));
}
