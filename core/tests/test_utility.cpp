#include "utility.h"
#include "gtest/gtest.h"

using namespace aria;

struct A {};
struct B {};

struct C {
  C() = default;
  C(const C &) = default;
  C &operator=(C &&) = delete;
};

TEST(test_pair, basic) {
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
  {
    pair<int, int> p(1, 2), q(0, 0);
    const auto p1 = p, q1 = q;
    EXPECT_EQ(p, p1);
    p = move(q);
    EXPECT_EQ(p, q1);
    q = p;
    EXPECT_EQ(q, q1);
  }
}

