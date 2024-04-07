#include "algorithm.h"
#include "list.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_algorithm, reverse) {
  {
    vector<int> v = {1, 2, 3}, u = {3, 2, 1};
    reverse(v.begin(), v.end());
    EXPECT_EQ(v, u);
  }
  {
    list<int> v = {1, 2, 3}, u = {3, 2, 1};
    reverse(v.begin(), v.end());
    EXPECT_EQ(v, u);
  }
}

TEST(test_algorithm, find) {
  {
    vector<int> v = {1, 2, 3};
    auto it = aria::find(v.begin(), v.end(), 2);
    EXPECT_EQ(it, v.begin() + 1);
  }
  {
    const vector<int> v = {1, 2, 3};
    auto it = aria::find_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    EXPECT_EQ(it, v.begin() + 1);
  }
}