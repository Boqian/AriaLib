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
  m.erase(3);
  EXPECT_EQ(m.size(), 1);
  auto it5 = m.find(5);
  m.erase(it5);
  EXPECT_EQ(m.size(), 0);
}

TEST(test_unordered_map, ctor) {
  {
    unordered_map<int, int> m = {{1, 2}, {4, 6}};
    EXPECT_EQ(m.size(), 2);
    EXPECT_TRUE(m.contains(1));
    EXPECT_TRUE(m.contains(4));
    EXPECT_TRUE(m[1] = 2);
    EXPECT_TRUE(m[4] = 6);
  }
  {
    vector<pair<int, int>> v = {{1, 2}, {4, 6}};
    unordered_map<int, int> m(v.begin(), v.end());
    EXPECT_EQ(m.size(), 2);
    EXPECT_TRUE(m[1] = 2);
    EXPECT_TRUE(m[4] = 6);
  }
}

TEST(test_unordered_map, more) {
  {
    unordered_map<int, int> a = {{1, 2}, {4, 6}};
    const auto b = a;
    EXPECT_EQ(a.at(1), 2);
    EXPECT_EQ(b.at(4), 6);
    a.clear();
    EXPECT_TRUE(a.empty());
  }
}