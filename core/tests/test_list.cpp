#include "gtest/gtest.h"
#include "list.h"

using namespace aria;

TEST(test_list, base) {
  list<int> li;
  li.push_back(10);
  EXPECT_EQ(*li.begin(), 10);
}