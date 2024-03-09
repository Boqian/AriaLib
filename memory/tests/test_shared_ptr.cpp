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

TEST(test_shared_ptr, copy_construct) {
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

TEST(test_shared_ptr, swap) {
  auto p = shared_ptr<int>(new int(100));
  auto q = shared_ptr<int>(new int(33));
  p.swap(q);
  EXPECT_EQ(*p, 33);
  EXPECT_EQ(*q, 100);
}

TEST(test_shared_ptr, reset) {
  auto p = shared_ptr<int>(new int(100));
  p.reset();
  EXPECT_FALSE(p);
  EXPECT_EQ(p.use_count(), 0);
}

TEST(test_shared_ptr, make_shard) {
  auto p = make_shared<std::pair<int, double>>(1, 1.3);
  EXPECT_EQ(p->first, 1);
  EXPECT_DOUBLE_EQ(p->second, 1.3);
}