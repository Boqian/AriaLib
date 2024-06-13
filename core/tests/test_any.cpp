#include "any.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(any, basic) {
  any x = 1;
  auto y = any_cast<int>(x);
  EXPECT_EQ(y, 1);
  try {
    auto z = any_cast<double>(x);
  } catch (bad_any_cast ex) {
    EXPECT_STREQ(ex.what(), "bad any cast");
  }
}