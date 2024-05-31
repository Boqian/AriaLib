#include "binary_search_tree.h"
#include "vector.h";
#include "gtest/gtest.h"
using namespace aria;

TEST(test_binary_search_tree, basic) {
  {
    binary_search_tree<int, int> tree;
    EXPECT_EQ(tree.begin(), tree.end());
    auto it = tree.find(10);
    EXPECT_EQ(it, tree.end());
  }
  {
    binary_search_tree<int, int> tree;
    auto [it, flag] = tree.insert({1, 2});
    EXPECT_TRUE(flag);
    EXPECT_TRUE(tree.contains(1));
    EXPECT_NE(tree.begin(), tree.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, 2);
    EXPECT_EQ(it, tree.find(1));
  }
  {
    binary_search_tree<int, void> tree;
    for (int i = 0; i < 10; i++) {
      EXPECT_EQ(tree.size(), i);
      EXPECT_FALSE(tree.contains(i));
      tree.insert(i);
      EXPECT_TRUE(tree.contains(i));
    }
  }
  {
    binary_search_tree<int, void> tree = {4};
    tree.insert(2);
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    auto it = tree.begin();
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(*(++it), 4);
  }
  {
    binary_search_tree<int, void> tree = {2, 4, 6, 1, 3, 5, 7, 0};
    vector<int> ans = {0, 1, 2, 3, 4, 5, 6, 7}, v;
    for (auto x : tree)
      v.push_back(x);
    EXPECT_EQ(v, ans);
  }
}