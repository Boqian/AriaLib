#include "array.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(random_access_iterator<array<int, 10>::iterator>);
static_assert(random_access_iterator<array<int, 7>::const_iterator>);
static_assert(tuple_size_v<array<int, 7>> == 7);

TEST(test_array, basic) {
  array<int, 3> v = {1, 2, 3};
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(v.size(), 3);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);
  EXPECT_EQ(v.front(), v[0]);
  EXPECT_EQ(v.back(), v[2]);
}

TEST(test_array, assign) {
  array<int, 2> a = {1, 2}, b = {5, 6};
  a = b;
  EXPECT_EQ(a, b);
  a[0] = 1;
  EXPECT_NE(a, b);
}

TEST(test_array, compare) {
  array<int, 2> a = {1, 2}, b = {1, 3}, c = {1, 2};
  EXPECT_EQ(a, c);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(b > a);
}

TEST(test_array, swap) {
  array<int, 2> a = {1, 2}, b = {5, 6}, a1 = a, b1 = b;
  swap(a, b);
  EXPECT_EQ(a, b1);
  EXPECT_EQ(b, a1);
}

TEST(test_array, iterator) {
  {
    array<int, 3> v = {1, 2, 3};
    auto it = v.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    it += 2;
    EXPECT_EQ(it, v.end());
    --it;
    EXPECT_EQ(*it, 3);
    it -= 2;
    EXPECT_EQ(it, v.begin());
    *it = 5;
    EXPECT_EQ(v[0], 5);
  }
  {
    array<int, 3> v = {1, 2, 3};
    for (auto &x : v) {
      x++;
    }
    array<int, 3> u = {2, 3, 4};
    EXPECT_EQ(v, u);
  }
}

TEST(test_array, to_array) {
  int a[3] = {1, 2, 3};
  auto v = to_array(a);
  array ans({1, 2, 3});
  EXPECT_EQ(v, ans);
}
