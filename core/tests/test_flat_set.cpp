#include "flat_set.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_flat_set, basic) {
  flat_set<int> st;
  st.insert(2);
  EXPECT_EQ(st, flat_set<int>{2});
  auto [_, flag] = st.insert(2);
  EXPECT_FALSE(flag);
  EXPECT_EQ(st, flat_set<int>{2});
  st.insert(1);
  EXPECT_EQ(st, (flat_set<int>{1, 2}));
  st.insert(3);
  EXPECT_EQ(st, (flat_set<int>{1, 2, 3}));
  for (int i : {1, 2, 3}) {
    EXPECT_NE(st.find(i), st.end());
    EXPECT_EQ(*st.find(i), i);
    EXPECT_TRUE(st.contains(i));
    EXPECT_EQ(st.count(i), 1);
  }
  st.erase(st.find(2));
  EXPECT_EQ(st, (flat_set<int>{1, 3}));
}

TEST(test_flat_multiset, basic) {
  flat_multiset<int> st = {2, 1, 2, 3, 2, 4};
  EXPECT_EQ(st.count(2), 3);
  st.insert(2);
  EXPECT_EQ(st.count(2), 4);
  auto v = move(st).extract();
  EXPECT_EQ(v, (vector<int>{1, 2, 2, 2, 2, 3, 4}));
}