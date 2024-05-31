#include "binary_search_tree.h"
#include "vector.h"
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
    auto it2 = tree.end();
    EXPECT_EQ(*(--it2), 4);
    EXPECT_EQ(*(--it2), 2);
    auto it3 = tree.rbegin();
    EXPECT_EQ(*it3, 4);
    EXPECT_EQ(*(++it3), 2);
  }
  {
    binary_search_tree<int, void> tree = {2, 4, 6, 1, 3, 5, 7, 0};
    auto d = distance(tree.begin(), tree.end());
    EXPECT_EQ(d, tree.size());
    vector<int> ans = {0, 1, 2, 3, 4, 5, 6, 7};
    EXPECT_EQ(vector<int>(tree.begin(), tree.end()), ans);
    vector<int> ans2 = {7, 6, 5, 4, 3, 2, 1, 0};
    EXPECT_EQ(vector<int>(tree.rbegin(), tree.rend()), ans2);
  }
  {
    binary_search_tree<int, void> tree = {2, 4, 6, 1, 3, 5, 7, 0};
    for (auto x : tree) {
      auto [it, flag] = tree.insert(x);
      EXPECT_EQ(*it, x);
      EXPECT_FALSE(flag);
    }
  }
}