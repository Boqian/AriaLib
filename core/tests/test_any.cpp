#include "any.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(any, basic) {
  {
    any x = 1;
    EXPECT_EQ(x.type(), typeid(int));
    EXPECT_EQ(any_cast<int>(x), 1);
    try {
      auto z = any_cast<double>(x);
    } catch (bad_any_cast ex) {
      EXPECT_STREQ(ex.what(), "bad any cast");
    }
  }
  {
    const any x = 1;
    auto p = any_cast<int>(&x);
    EXPECT_EQ(*p, 1);
  }
  {
    any x = 1;
    auto p = any_cast<int>(&x);
    EXPECT_EQ(*p, 1);
    *p = 99;
    EXPECT_EQ(any_cast<int>(x), 99);
  }
  {
    any x = vector<int>{1, 2, 3};
    auto v = any_cast<vector<int>>(move(x));
    EXPECT_EQ(v, (vector<int>{1, 2, 3}));
    EXPECT_EQ(any_cast<vector<int>>(x), (vector<int>{})); // moved
  }
}

TEST(any, rule5) {
  {
    any x = 100;
    auto y = x;
    EXPECT_EQ(x.type(), typeid(int));
    EXPECT_EQ(y.type(), typeid(int));
    EXPECT_EQ(any_cast<int>(x), 100);
    EXPECT_EQ(any_cast<int>(y), 100);
  }
  {
    any x = 100;
    x = 1.5;
    EXPECT_EQ(x.type(), typeid(double));
    EXPECT_DOUBLE_EQ(any_cast<double>(x), 1.5);
  }
}