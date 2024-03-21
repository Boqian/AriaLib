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
  {
    pair<int, int> p(1, 2), q(3, 3);
    swap(p, q);
    EXPECT_EQ(p.first, 3);
    EXPECT_EQ(q.first, 1);
  }
  {
    pair p(1, 2.5);
    auto [x, y] = p;
    EXPECT_EQ(x, 1);
    EXPECT_DOUBLE_EQ(y, 2.5);
  }
}

TEST(test_tuple, basic) {
  { tuple<int, double, A> tp; }
  {
    tuple<int, double, A> tp(1, 2.5, A{});
    static_assert(tuple_size_v<decltype(tp)> == 3);
    EXPECT_EQ(get<0>(tp), 1);
    EXPECT_EQ(get<int>(tp), 1);
    EXPECT_DOUBLE_EQ(get<1>(tp), 2.5);
    get<0>(tp) = 123;
    EXPECT_EQ(get<0>(tp), 123);
    EXPECT_EQ(get<int>(tp), 123);
    EXPECT_DOUBLE_EQ(get<double>(tp), 2.5);
    get<double>(tp) = 4.8;
    EXPECT_DOUBLE_EQ(get<1>(tp), 4.8);
  }
  {
    constexpr tuple<int, double> tp(1, 2.5);
    static_assert(tuple_size_v<decltype(tp)> == 2);
    static_assert(get<0>(tp) == 1);
    static_assert(get<int>(tp) == 1);
  }
  {
    constexpr tuple<int, int> a(1, 1), b(2, 0), c(1, 1);
    static_assert(a < b);
    static_assert(a == c);
  }
  {
    using T = tuple<int, double, A>;
    static_assert(is_same_v<int, tuple_element<0, T>::type>);
    static_assert(is_same_v<double, tuple_element<1, T>::type>);
    static_assert(is_same_v<A, tuple_element<2, T>::type>);
  }
  {
    tuple<int, int> a(1, 1), b(2, 0), a1 = a, b1 = b;
    swap(a, b);
    EXPECT_EQ(a, b1);
    EXPECT_EQ(a1, b);
  }
  {
    tuple<int, double> a(1, 1), b;
    b = a;
    EXPECT_EQ(a, b);
    auto c = a;
    EXPECT_EQ(a, c);
  }
  {
    auto t = make_tuple(1, 2.5, A{});
    EXPECT_EQ(get<0>(t), 1);
    EXPECT_DOUBLE_EQ(get<double>(t), 2.5);
  }
  { 
      tuple<int, int> p('a', 1u);
  }
}