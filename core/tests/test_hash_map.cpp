#include "myhash.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_hash_map, base) {
  my_hash<int, int> m;
  EXPECT_TRUE(m.empty());
}