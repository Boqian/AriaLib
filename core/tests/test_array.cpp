#include "array.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(random_access_iterator<array<int, 10>::iterator>);
static_assert(random_access_iterator<array<int, 7>::const_iterator>);

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

TEST(test_array, compare) {
  array<int, 2> a = {1, 2}, b = {1, 3}, c = {1, 2};
  EXPECT_TRUE(a == c);
  EXPECT_TRUE(a < b);
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