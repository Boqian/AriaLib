#include "algorithm.h"
#include "list.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_algorithm, search_operations) {
  const vector<int> v = {1, 2, 3, 4, 5};
  EXPECT_TRUE(all_of(v.begin(), v.end(), [](int x) { return x <= 5; }));
  EXPECT_FALSE(all_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
  EXPECT_FALSE(none_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
  EXPECT_TRUE(any_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
  EXPECT_TRUE(none_of(v.begin(), v.end(), [](int x) { return x > 5; }));
}

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

TEST(test_algorithm, minmax) {
  {
    vector<int> v = {4, 1, 3, 2};
    auto x = *min_element(v.begin(), v.end());
    EXPECT_EQ(x, 1);
    auto y = *min_element(v.begin(), v.end(), [](int a, int b) { return a > b; });
    EXPECT_EQ(y, 4);
  }
  {
    EXPECT_EQ(aria::min(1, 5), 1);
    EXPECT_EQ(aria::min({3, 4, 5}), 3);
  }
  {
    vector<int> v = {1, 4, 3, 2};
    auto x = *max_element(v.begin(), v.end());
    EXPECT_EQ(x, 4);
    auto y = *max_element(v.begin(), v.end(), [](int a, int b) { return a > b; });
    EXPECT_EQ(y, 1);
  }
  {
    EXPECT_EQ(aria::max(1, 5), 5);
    EXPECT_EQ(aria::max({3, 4, 5}), 5);
  }
}