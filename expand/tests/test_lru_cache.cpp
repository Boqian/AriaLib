#include "lru_cache.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_lru_cache, basic) {
  int cap = 2;
  LRUCache<int, int> cache(cap);
  cache.update(1, 5);
  EXPECT_EQ(cache.get(1).value(), 5);
  EXPECT_FALSE(cache.get(2).has_value());
  cache.update(2, 3);
  EXPECT_EQ(cache.get(2).value(), 3);
  cache.update(3, 6);
  EXPECT_FALSE(cache.get(1).has_value());
}