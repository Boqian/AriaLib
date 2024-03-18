#include "functional.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_function, basic) {
  auto lambda = [](int x) { return x * 2; };
  function<int(int)> f(lambda);
  EXPECT_EQ(f(100), 200);
  EXPECT_EQ(f(-1), -2);
}