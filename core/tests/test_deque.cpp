#include "deque.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_deque, basic) {
  {
    deque<int> deq;
    EXPECT_TRUE(deq.empty());
    EXPECT_EQ(deq.size(), 0);
    deq.push_back(44);
    EXPECT_FALSE(deq.empty());
    EXPECT_EQ(deq.size(), 1);
    EXPECT_EQ(deq[0], 44);
    deq[0] = 55;
    EXPECT_EQ(deq[0], 55);
  }
  {
    deque<int> deq;
    const int n = 1000;
    for (int i = 0; i < n; i++) {
      deq.push_back(i * 10);
      EXPECT_EQ(deq.size(), i + 1);
      EXPECT_EQ(deq[i], i * 10);
    }
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(deq[i], i * 10);
    }
  }
  {
    deque<int> deq;
    const int n = 1000;
    for (int i = 0; i < n; i++) {
      deq.push_front(i * 10);
      EXPECT_EQ(deq.size(), i + 1);
      EXPECT_EQ(deq[0], i * 10);
    }
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(deq[i], (n - i - 1) * 10);
    }
  }
  {
    deque<int> deq = {1, 2, 3, 4, 5};
    EXPECT_EQ(deq.size(), 5);
    EXPECT_EQ(deq.front(), 1);
    EXPECT_EQ(deq.back(), 5);
  }
}
