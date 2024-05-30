#include "hash_table.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(bidirectional_iterator<hash_table<int, int>::iterator>);
static_assert(bidirectional_iterator<hash_table<int, int>::const_iterator>);

TEST(test_hash_map, _KeyVal) {
  auto kv = pair(1, 2);
  EXPECT_EQ(_hash_table::get_key(kv), 1);
}

TEST(test_hash_map, basic) {
  hash_table<int, int> m;
  EXPECT_TRUE(m.empty());

  const int n = 70;
  for (int i = 0; i < n; i++) {
    auto [it, flag] = m.insert(pair(i * 2, i * 4));
    EXPECT_TRUE(flag) << i;
  }

  for (int i = 0; i < n; i++) {
    auto [it, flag] = m.insert(pair(i * 2, i * 10));
    EXPECT_FALSE(flag) << i;
  }

  EXPECT_EQ(m.size(), n);

  for (int i = 0; i < n; i++) {
    EXPECT_TRUE(m.contains(i * 2));
    EXPECT_FALSE(m.contains(i * 2 + 1));
  }

  for (int i = 0; i < n; i++) {
    int cnt = m.erase(i * 2);
    EXPECT_EQ(cnt, 1);
    cnt = m.erase(i * 2);
    EXPECT_EQ(cnt, 0);
    cnt = m.erase(i * 2 + 1);
    EXPECT_EQ(cnt, 0);
  }

  EXPECT_TRUE(m.empty());
}

TEST(test_hash_map, more_test) {
  hash_table<int, int> m = {{1, 2}, {3, 4}};
  EXPECT_EQ(m.size(), 2);
  m.clear();
  EXPECT_EQ(m.size(), 0);
}