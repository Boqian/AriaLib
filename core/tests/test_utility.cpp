#include "utility.h"
#include "gtest/gtest.h"

using namespace aria;

struct A {};
struct B {};

TEST(test_utility, pair) {
  {
    pair<int, int> p(5, 6);
    EXPECT_EQ(p.first, 5);
    EXPECT_EQ(p.second, 6);
    p.first = 4;
    EXPECT_EQ(p.first, 4);
  }
  {
    pair p(A{}, B{});
    static_assert(is_same_v<decltype(p)::first_type, A>);
    static_assert(is_same_v<decltype(p)::second_type, B>);
    static_assert(is_same_v<decltype(p.first), A>);
    static_assert(is_same_v<decltype(p.second), B>);
  }
  {
    auto p = make_pair(1, 2.5);
    EXPECT_EQ(p.first, 1);
    EXPECT_DOUBLE_EQ(p.second, 2.5);
    auto q = make_pair(A(), B());
  }
  {
    auto p = make_pair(1, 2);
    auto q = make_pair(2, 1);
    EXPECT_TRUE(p != q);
    EXPECT_TRUE(p < q);
    EXPECT_TRUE(q > p);
    auto r = make_pair(1, 2);
    EXPECT_TRUE(p == r);
  }
}
