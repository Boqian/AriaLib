#include "myhash.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_hash_map, _KeyVal) {
  _KeyVal<int, int> kv = {pair<const int, int>(1, 2)};
  EXPECT_EQ(kv.key(), 1);
}

TEST(test_hash_map, base) {
  my_hash<int, int> m;
  EXPECT_TRUE(m.empty());
  m.insert(pair<const int, int>(1, 2));
  EXPECT_TRUE(m.find(1));
  EXPECT_FALSE(m.find(2));
}