#include "bitset.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_bitset, basic) {
  {
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
  {
    bitset<4> bt;
    EXPECT_EQ(bt.to_ullong(), 0);
    bt[0] = 1;
    EXPECT_EQ(bt.to_ullong(), 1);
    bt[1] = 1;
    EXPECT_EQ(bt.to_ullong(), 3);
    bt[2] = 1;
    EXPECT_EQ(bt.to_ullong(), 7);
  }
  {
    bitset<4> bt(2);
    EXPECT_EQ(bt.to_ullong(), 2);
    bt.flip(0);
    EXPECT_EQ(bt.to_ullong(), 3);
    bt.flip(1);
    EXPECT_EQ(bt.to_ullong(), 1);
  }
}
