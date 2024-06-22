#include "span.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_span, constructor) {
  {
    array<int, 3> v = {1, 2, 3};
    span sp(v);
    EXPECT_EQ(sp.front(), 1);
    EXPECT_EQ(sp.back(), 3);
    EXPECT_EQ(sp.size(), 3);
    sp[0] = 10;
    EXPECT_EQ(v[0], 10);
  }
  {
    vector<int> v = {1, 2, 3};
    span sp(v);
    EXPECT_EQ(sp.front(), 1);
    EXPECT_EQ(sp.back(), 3);
    EXPECT_EQ(sp.size(), 3);
    sp[0] = 10;
    EXPECT_EQ(v[0], 10);
  }
}

TEST(test_span, basic) {
  {
    vector<int> v = {1, 2, 3};
    span sp(v);
    EXPECT_EQ(*sp.begin(), 1);
    EXPECT_EQ(*sp.rbegin(), 3);
    for (auto &x : sp)
      x += 1;
    EXPECT_EQ(v, (vector<int>{2, 3, 4}));
  }
  {
    vector<int> v = {1, 2, 3, 4};
    span sp(v);
    auto sub1 = sp.first<2>();
    EXPECT_EQ(sub1.size(), 2);
    EXPECT_EQ(sub1[0], 1);
    EXPECT_EQ(sub1[1], 2);
    auto sub2 = sp.first(2);
    EXPECT_EQ(sub2.size(), 2);
    EXPECT_EQ(sub2[0], 1);
    EXPECT_EQ(sub2[1], 2);
  }
}