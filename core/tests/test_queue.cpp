#include "queue.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_queue, basic) {
  queue<int> q;
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(q.size(), 0);
  q.push(5);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(q.size(), 1);
  EXPECT_EQ(q.front(), 5);
  q.push(4);
  q.push(3);
  EXPECT_EQ(q.size(), 3);
  EXPECT_EQ(q.front(), 5);
  q.pop();
  EXPECT_EQ(q.size(), 2);
  EXPECT_EQ(q.front(), 4);
  q.pop();
  EXPECT_EQ(q.size(), 1);
  EXPECT_EQ(q.front(), 3);
  q.pop();
  EXPECT_EQ(q.size(), 0);
}