#include "set.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_set, basic) {
  {
    set<int> st;
    EXPECT_TRUE(st.empty());
    auto [it, flag] = st.insert(3);
    EXPECT_TRUE(flag);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(st.size(), 1);
    auto it2 = st.find(3);
    EXPECT_TRUE(it2 != st.end());
    EXPECT_EQ(*it2, 3);
    EXPECT_EQ(it2, it);
    auto [it3, flag2] = st.insert(3);
    EXPECT_FALSE(flag2);
  }
  {
    const int n = 100;
    set<int> st;
    for (int i = 0; i < n; i++) {
      if (i % 2 == 0)
        st.insert(i);
    }
    EXPECT_EQ(st.size(), n / 2);
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(st.contains(i), (i % 2 == 0)) << i;
    }
  }
  {
    set<int> st = {1, 2, 3};
    EXPECT_EQ(st.size(), 3);
    int sum = 0;
    for (auto x : st)
      sum += x;
    EXPECT_EQ(sum, 6);
  }
  {
    set<int> a = {1, 2, 3}, b = {1, 2, 3}, c = {1, 2, 4};
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    swap(a, c);
    EXPECT_EQ(c, b);
  }
  {
    set<int> a = {1, 2, 3, 4, 5, 6}, b = {2, 4, 6};
    erase_if(a, [](auto &key) { return key % 2 == 1; });
    EXPECT_EQ(a, b);
  }
}
