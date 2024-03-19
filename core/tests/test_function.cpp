#include "functional.h"
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

TEST(test_function, ctor_assign) {
  function<int(int)> f([](int x) { return x * 2; });
  function<int(int)> g(std::move(f));
  EXPECT_FALSE(f);
  EXPECT_TRUE(g);
  EXPECT_EQ(g(3), 6);
}