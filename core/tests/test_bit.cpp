#include "bit.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_bit, popcount) {
  EXPECT_EQ(2, popcount(3u));
  EXPECT_EQ(2, popcount(3ul));
  EXPECT_EQ(2, popcount(3ull));
  EXPECT_EQ(2, popcount(unsigned char(3)));

  unsigned long long x = (1ull << 36) - 1;
  EXPECT_EQ(36, popcount(x));
}

TEST(test_bit, has_single_bit) {
  for (unsigned int i = 0; i < 100; i++) {
    EXPECT_EQ(has_single_bit(i), popcount(i) == 1);
  }
}

TEST(test_bit, bit_width) {
  EXPECT_EQ(bit_width(4u), 3);
  EXPECT_EQ(bit_width(8u), 4);
  EXPECT_EQ(bit_width(7u), 3);
  EXPECT_EQ(bit_width(0u), 0);
  EXPECT_EQ(bit_width(1u), 1);
}

TEST(test_bit, bit_ceil) {
  EXPECT_EQ(bit_ceil(0u), 1);
  EXPECT_EQ(bit_ceil(1u), 1);
  EXPECT_EQ(bit_ceil(4u), 4);
  EXPECT_EQ(bit_ceil(7u), 8);
  EXPECT_EQ(bit_ceil(120u), 128);
}

TEST(test_bit, bit_floor) {
  EXPECT_EQ(bit_floor(0u), 0);
  EXPECT_EQ(bit_floor(1u), 1);
  EXPECT_EQ(bit_floor(4u), 4);
  EXPECT_EQ(bit_floor(7u), 4);
  EXPECT_EQ(bit_floor(120u), 64);
}