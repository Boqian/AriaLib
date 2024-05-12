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
    bt.flip();
    EXPECT_EQ(bt.to_ullong(), 14);
  }
  {
    bitset<4> bt(2);
    bt.set();
    EXPECT_EQ(bt.to_ullong(), 15);
    bt.reset();
    EXPECT_EQ(bt.to_ullong(), 0);
  }
  {
    bitset<10> a(123);
    auto b = a;
    EXPECT_EQ(b.to_ullong(), 123);
  }
  {
    bitset<4> a(1);
    EXPECT_EQ((~a).to_ullong(), 14);
    EXPECT_EQ(a.to_ullong(), 1);
  }
  {
    bitset<3> a(7);
    EXPECT_TRUE(a.all());
  }
  {
    bitset<3> a(0);
    EXPECT_TRUE(a.none());
  }
  {
    bitset<3> a(0);
    EXPECT_FALSE(a.any());
    a[1] = 1;
    EXPECT_TRUE(a.any());
  }
  {
    bitset<4> a(5), b(5), c(6);
    EXPECT_EQ(a, b);
    EXPECT_TRUE(a != c);
  }
}
