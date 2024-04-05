#include "optional.h"
#include "utility.h"
#include "gtest/gtest.h"

struct CC {
  CC(int a) : x(a) {}
  CC(CC &&rhs) : x(rhs.x) { rhs.x = 0; }
  CC &operator=(CC &&rhs) {
    x = rhs.x;
    rhs.x = 0;
    return *this;
  }
  int x = 0;
};

using namespace aria;

TEST(test_optional, basic) {
  {
    optional<int> a;
    EXPECT_FALSE(a);
    constexpr optional<int> b;
    static_assert(!b);
  }
  {
    optional<int> a(5);
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, 5);
    constexpr optional<int> b(4);
    static_assert(*b == 4);
  }
  {
    optional<pair<int, double>> a(pair(1, 1.5));
    EXPECT_EQ(a->first, 1);
    EXPECT_DOUBLE_EQ(a->second, 1.5);
    a->first = 10;
    EXPECT_EQ(a->first, 10);
  }
  {
    optional<int> a(5);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), 5);
    EXPECT_EQ(a.value_or(3), 5);
    a.value() = 10;
    EXPECT_EQ(a.value(), 10);
    optional<int> b;
    EXPECT_EQ(b.value_or(3), 3);
  }
  {
    optional<int> a(5), b, c;
    b = a;
    EXPECT_EQ(b.value(), 5);
    b = c;
    EXPECT_FALSE(b);
  }
  {
    optional<int> a(5);
    auto b = a;
    EXPECT_EQ(b.value(), 5);
  }
  {
    optional<int> a(5);
    optional<int> b = move(a);
    EXPECT_EQ(b.value(), 5);
    EXPECT_EQ(a.value(), 5);
  }
  {
    optional<CC> a(123);
    optional<CC> b = move(a);
    EXPECT_EQ(b->x, 123);
    EXPECT_EQ(a->x, 0);
  }
  {
    optional<CC> a(123), b;
    b = move(a);
    EXPECT_EQ(b->x, 123);
    EXPECT_EQ(a->x, 0);
  }
}

struct A {
  inline static int n_ctor = 0;
  inline static int n_copy_ctor = 0;
  inline static int n_copy_assign = 0;
  inline static int n_dtor = 0;
  static void reset() {
    n_ctor = 0;
    n_copy_ctor = 0;
    n_copy_assign = 0;
    n_dtor = 0;
  }

  A() { n_ctor++; }
  A(int) { n_ctor++; }
  A(const A &) { n_copy_ctor++; }
  A &operator=(const A &) {
    n_copy_assign++;
    return *this;
  }
  ~A() { n_dtor++; }
};

TEST(test_optional, ctor) {
  A a;
  A::reset();
  {
    optional<A> b(a);
    EXPECT_EQ(A::n_copy_ctor, 1);
    EXPECT_EQ(A::n_copy_assign, 0);

    optional<A> c;
    c = b;
    EXPECT_EQ(A::n_copy_ctor, 2);
    EXPECT_EQ(A::n_copy_assign, 0);
  }
  EXPECT_EQ(A::n_dtor, 2);
  {
    optional<A> b(a), c(a);
    A::reset();
    c = b;
    EXPECT_EQ(A::n_copy_ctor, 0);
    EXPECT_EQ(A::n_copy_assign, 1);
  }
  EXPECT_EQ(A::n_dtor, 2);
  {
    A::reset();
    optional<A> b(1);
    EXPECT_EQ(A::n_copy_ctor, 0);
    EXPECT_EQ(A::n_ctor, 1);
  }
  EXPECT_EQ(A::n_dtor, 1);
}

TEST(test_optional, swap) {
  optional<CC> a(1), b(2), c, d;
  swap(a, b);
  EXPECT_TRUE(a->x == 2 && b->x == 1);
  swap(a, c);
  EXPECT_FALSE(a);
  EXPECT_EQ(c->x, 2);
}