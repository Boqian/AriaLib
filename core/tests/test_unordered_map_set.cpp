#include "unordered_map.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_unordered_map, basic) {
  unordered_map<int, int> m;
  m.insert(pair(3, 2));
  EXPECT_EQ(m.size(), 1);
  EXPECT_EQ(m[3], 2);
  m[3] = 4;
  EXPECT_EQ(m[3], 4);
  auto it = m.find(3);
  EXPECT_TRUE(it != m.end());
  m[5] = 10;
  EXPECT_EQ(m.size(), 2);
  EXPECT_EQ(m[5], 10);
}