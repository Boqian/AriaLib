#include "../unique_ptr.h"
#include "gtest/gtest.h"
#include <utility>


using namespace aria;

TEST(test_unique_ptr, basic) {
  {
    auto p = unique_ptr<int>();
    EXPECT_FALSE(p);
  }
  {
    auto p = unique_ptr<int>(new int(100));
    EXPECT_TRUE(p);
    EXPECT_EQ(*p, 100);
    *p = 50;
    EXPECT_EQ(*p, 50);
    p.reset();
    EXPECT_FALSE(p);
  }
  {
    auto p = unique_ptr<int>(new int(100));
    p = nullptr;
    EXPECT_FALSE(p);
  }
  {
    auto p = unique_ptr<int>(new int(100));
    auto q = unique_ptr<int>(new int(50));
    p.swap(q);
    EXPECT_EQ(*p, 50);
    EXPECT_EQ(*q, 100);
    p = std::move(q);
    EXPECT_FALSE(q);
    EXPECT_EQ(*p, 100);
  }
}


TEST(test_unique_ptr, make_unique) {
  auto p = make_unique<int>(1);
  EXPECT_EQ(*p, 1);

  auto q = make_unique<std::pair<int, double>>(1, 2.2);
  EXPECT_EQ(q->first, 1);
  EXPECT_DOUBLE_EQ(q->second, 2.2);
}