#include "list.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_iterator, distance) {
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(distance(v.begin(), v.end()), 4);
  }
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(advance(v.begin(), 4), v.end());
    EXPECT_EQ(advance(v.end(), -4), v.begin());
  }
  {
    list<int> v = {1, 2, 3, 4};
    EXPECT_EQ(advance(v.begin(), 4), v.end());
    EXPECT_EQ(advance(v.end(), -4), v.begin());
  }
  {
    vector<int> v = {1, 2, 3, 4};
    auto it = v.begin(), it2 = next(it), it3 = prev(it2);
    EXPECT_EQ(*it2, 2);
    EXPECT_EQ(it3, it);
  }
}

TEST(test_iterator, begin_end) {
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(begin(v), v.begin());
    EXPECT_EQ(end(v), v.end());
  }
}