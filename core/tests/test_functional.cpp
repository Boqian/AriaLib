#include "functional.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_function, basic) {
  auto lambda = [](int x) { return x * 2; };
  function<int(int)> f(lambda);
  EXPECT_EQ(f(100), 200);
  EXPECT_EQ(f(-1), -2);
}

TEST(test_function, bool_operator) {
  function<void(int)> f;
  EXPECT_FALSE(f);
}

TEST(test_function, move_ctor) {
  function<int(int)> f([](int x) { return x * 2; });
  function<int(int)> g(move(f));
  EXPECT_FALSE(f);
  EXPECT_TRUE(g);
  EXPECT_EQ(g(3), 6);
}

TEST(test_function, move_assign) {
  function<int(int)> f([](int x) { return x * 2; });
  function<int(int)> g;
  g = move(f);
  EXPECT_FALSE(f);
  EXPECT_TRUE(g);
  EXPECT_EQ(g(3), 6);
}

int foo(int x) { return x * 3; }

TEST(test_function, free_function) {
  function<int(int)> f(foo);
  EXPECT_EQ(f(100), 300);
}

TEST(test_reference_wrapper, basic) {
  {
    using int_ref = ::aria::reference_wrapper<int>;
    int x = 5;
    int_ref rw(x);
    vector<int_ref> v;
    v.emplace_back(rw);
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[0].get(), 5);
    v[0].get() = 2;
    EXPECT_EQ(v[0].get(), 2);
    EXPECT_EQ(x, 2);
    int y = 6;
    v.emplace_back(y);
    v[1].get() = -1;
    EXPECT_EQ(y, -1);
    v.emplace_back(aria::ref(y));
    EXPECT_EQ(v[2].get(), -1);
    y = 10;
    EXPECT_EQ(v[2], 10);
    EXPECT_EQ(v[1], 10);
  }
  {
    using int_cref = ::aria::reference_wrapper<const int>;
    vector<int_cref> v;
    int x = 5;
    v.emplace_back(cref(x));
    EXPECT_EQ(v[0], 5);
    x = 6;
    EXPECT_EQ(v[0], 6);
  }
}

TEST(test_hash, basic) {
  {
    int x = 5, y = 5;
    EXPECT_EQ(hash<int>{}(x), hash<int>{}(y));
  }
}

TEST(test_functional, functor) {
  EXPECT_TRUE(less()(1, 2));
  EXPECT_TRUE(greater()(2, 1));
  EXPECT_TRUE(equal_to()(2, 2));
  EXPECT_TRUE(not_equal_to()(2, 1));

  EXPECT_EQ(plus()(1, 2), 3);
  EXPECT_EQ(minus()(1, 2), -1);
  EXPECT_EQ(multiplies()(3, 2), 6);
  EXPECT_EQ(divides()(15, 3), 5);
  EXPECT_EQ(modulus()(17, 5), 2);
  EXPECT_EQ(negate()(8), -8);
}
