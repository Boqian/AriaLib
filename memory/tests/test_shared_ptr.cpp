#include "shared_ptr.h"
#include "gtest/gtest.h"
#include <utility>

using namespace aria;
TEST(test_shared_ptr, empty) {
  {
    auto p = shared_ptr<int>();
    EXPECT_FALSE(p);
    EXPECT_EQ(p.use_count(), 0);
  }
}

TEST(test_shared_ptr, construct) {
  {
    auto p = shared_ptr<int>(new int(100));
    EXPECT_TRUE(p);
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_EQ(*p, 100);
    {
      auto q = p;
      EXPECT_TRUE(q);
      EXPECT_EQ(p.use_count(), 2);
      EXPECT_EQ(q.use_count(), 2);
      EXPECT_EQ(*q, 100);
    }
    EXPECT_EQ(p.use_count(), 1);
  }
}