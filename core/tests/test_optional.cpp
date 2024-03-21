#include "gtest/gtest.h"
#include "optional.h"
#include "utility.h"

using namespace aria;

TEST(test_optional, basic) {
  {
    optional<int> a;
    EXPECT_FALSE(a);
    constexpr optional<int> b;
    static_assert(!b);
  }
  {
    optional<int> a(5);
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, 5);
    constexpr optional<int> b(4);
    static_assert(*b == 4);
  }
  {
    optional<pair<int, double>> a(pair(1, 1.5));
    EXPECT_EQ(a->first, 1);
    EXPECT_DOUBLE_EQ(a->second, 1.5);
    a->first = 10;
    EXPECT_EQ(a->first, 10);
  }
  {
    optional<int> a(5);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), 5);
    EXPECT_EQ(a.value_or(3), 5);
    a.value() = 10;
    EXPECT_EQ(a.value(), 10);
    optional<int> b;
    EXPECT_EQ(b.value_or(3), 3);
  }
  {
    optional<int> a(5), b, c;
    b = a;
    EXPECT_EQ(b.value(), 5);
    b = c;
    EXPECT_FALSE(b);
  }
}