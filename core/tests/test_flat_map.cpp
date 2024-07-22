#include "flat_map.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_flat_map, iterator) {
  vector<int> v = {1, 2, 3}, u = {4, 5, 6};
  joint_iterator it(v.begin(), u.begin());
  EXPECT_EQ(it->first, 1);
  EXPECT_EQ(it->second, 4);
  ++it;
  EXPECT_EQ(it->first, 2);
  EXPECT_EQ(it->second, 5);
}

TEST(test_flat_map, basic) { flat_map<int, int> m; }