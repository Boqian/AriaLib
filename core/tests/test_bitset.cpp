#include "bitset.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_bitset, basic) {
  bitset<4> bt;
  EXPECT_EQ(bt.to_ullong(), 0);
  bt.set(0, true);
  EXPECT_EQ(bt.to_ullong(), 1);
  bt.set(1, true);
  EXPECT_EQ(bt.to_ullong(), 3);
  bt.set(0, false);
  EXPECT_EQ(bt.to_ullong(), 2);
  bt.set(1, false);
  EXPECT_EQ(bt.to_ullong(), 0);
}
