#include "binary_search_tree.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_binary_search_tree, basic) {
  binary_search_tree<int, int> tree;
  auto it = tree.find(10);
  EXPECT_EQ(it, tree.end());
}