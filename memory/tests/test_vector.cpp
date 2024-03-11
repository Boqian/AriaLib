#include <utility>

#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_vector, base) {
  vector<int> v(2, 100);
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(v[0], 100);
  EXPECT_EQ(v[1], 100);
  v.push_back(33);
  EXPECT_EQ(v[2], 33);
  EXPECT_EQ(v.size(), 3);
  v.pop_back();
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(v.at(1), 100);
}

TEST(test_vector, construct) { auto v = vector<int>({1, 2, 3});
  EXPECT_EQ(v.size(), 3);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);

  auto u = v;
  EXPECT_EQ(u.size(), 3);
  EXPECT_EQ(u[0], 1);
  EXPECT_EQ(u[1], 2);
  EXPECT_EQ(u[2], 3);
}