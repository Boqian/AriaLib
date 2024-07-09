#include "expected.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_expected, basic) {
  {
    auto e = unexpected<int>(123);

    EXPECT_EQ(e.err(), 123);
  }
  {
    expected<int, double> a;
    EXPECT_TRUE(a);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), int{});
  }
  {
    expected<int, double> a = 5;
    EXPECT_EQ(a.value(), 5);
    expected<int, double> b = 'a';
    EXPECT_EQ(b.value(), 'a');
  }
}