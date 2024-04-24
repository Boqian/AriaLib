#include "list.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_iterator, distance) {
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(distance(v.begin(), v.end()), 4);
  }
}