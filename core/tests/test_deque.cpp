#include "deque.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_deque, basic) {
  deque<int> deq;
  EXPECT_TRUE(deq.empty());
  EXPECT_EQ(deq.size(), 0);
  deq.push_back(44);
  EXPECT_FALSE(deq.empty());
  EXPECT_EQ(deq.size(), 1);
  EXPECT_EQ(deq[0], 44);
}
