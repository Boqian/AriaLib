#include "span.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

template <class T> bool operator==(const span<T> &a, const span<T> &b) { return (a.data() == b.data()) && (a.size() == b.size()); }

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
  {
    vector<int> v = {1, 2, 3};
    span sp1(v.begin(), 3);
    span sp2(v.begin(), v.end());
    span sp3(v);
    EXPECT_TRUE(sp1 == sp2);
    EXPECT_TRUE(sp1 == sp3);
    EXPECT_TRUE(sp2 == sp3);
    span<int> sp4;
    sp4 = sp1;
    EXPECT_TRUE(sp2 == sp4);
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
  {
    vector<int> v = {1, 2, 3, 4};
    span sp(v);
    auto sub1 = sp.last<2>();
    EXPECT_EQ(sub1.size(), 2);
    EXPECT_EQ(sub1[0], 3);
    EXPECT_EQ(sub1[1], 4);
    auto sub2 = sp.last(2);
    EXPECT_EQ(sub2.size(), 2);
    EXPECT_EQ(sub2[0], 3);
    EXPECT_EQ(sub2[1], 4);
  }
  {
    vector<int> v = {1, 2, 3, 4};
    span sp(v);
    auto sub1 = sp.subspan<1, 2>();
    static_assert(is_same_v<decltype(sub1), span<int, 2>>);
    EXPECT_EQ(sub1[0], 2);
    EXPECT_EQ(sub1.size(), 2);
    auto sub2 = sp.subspan<1>();
    static_assert(is_same_v<decltype(sub2), span<int>>);
    EXPECT_EQ(sub2[0], 2);
    EXPECT_EQ(sub2.size(), 3);
  }
  {
    vector<int> v = {1, 2, 3, 4};
    span sp(v);
    auto sub1 = sp.subspan(1, 2);
    static_assert(is_same_v<decltype(sub1), span<int>>);
    EXPECT_EQ(sub1[0], 2);
    EXPECT_EQ(sub1.size(), 2);
    auto sub2 = sp.subspan(1);
    static_assert(is_same_v<decltype(sub2), span<int>>);
    EXPECT_EQ(sub2[0], 2);
    EXPECT_EQ(sub2.size(), 3);
  }
}