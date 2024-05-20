#include "string_view.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_string_view, basic) {
  string_view sv;
  EXPECT_TRUE(sv.empty());
  EXPECT_EQ(sv.size(), 0);
}