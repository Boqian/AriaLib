#include "unordered_map.h"
#include "unordered_set.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_unordered_map, basic) {
  unordered_map<int, int> m;
  m.insert(pair(3, 2));
  EXPECT_EQ(m.size(), 1);
  EXPECT_EQ(m[3], 2);
  m[3] = 4;
  EXPECT_EQ(m[3], 4);
  auto it = m.find(3);
  EXPECT_TRUE(it != m.end());
  m[5] = 10;
  EXPECT_EQ(m.size(), 2);
  EXPECT_EQ(m[5], 10);
  m.erase(3);
  EXPECT_EQ(m.size(), 1);
  auto it5 = m.find(5);
  m.erase(it5);
  EXPECT_EQ(m.size(), 0);
}

TEST(test_unordered_map, ctor) {
  {
    unordered_map<int, int> m = {{1, 2}, {4, 6}};
    EXPECT_EQ(m.size(), 2);
    EXPECT_TRUE(m.contains(1));
    EXPECT_TRUE(m.contains(4));
    EXPECT_TRUE(m[1] = 2);
    EXPECT_TRUE(m[4] = 6);
  }
  {
    vector<pair<int, int>> v = {{1, 2}, {4, 6}};
    unordered_map<int, int> m(v.begin(), v.end());
    EXPECT_EQ(m.size(), 2);
    EXPECT_TRUE(m[1] = 2);
    EXPECT_TRUE(m[4] = 6);
  }
}

TEST(test_unordered_map, more) {
  {
    unordered_map<int, int> a = {{1, 2}, {4, 6}};
    const auto b = a;
    EXPECT_EQ(a.at(1), 2);
    EXPECT_EQ(b.at(4), 6);
    a.clear();
    EXPECT_TRUE(a.empty());
  }
  {
    unordered_map<int, int> a = {{1, 2}}, b = {{1, 2}}, c = {{1, 3}};
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    swap(a, c);
    EXPECT_EQ(c, b);
  }
  {
    unordered_map<int, int> a = {{1, 2}, {4, 6}, {3, 1}}, b = {{4, 6}};
    erase_if(a, [](auto &kv) { return kv.first % 2 == 1; });
    EXPECT_EQ(a, b);
  }
}

TEST(test_unordered_set, basic) {
  {
    unordered_set<int> st;
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
    unordered_set<int> st;
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
    unordered_set<int> st = {1, 2, 3};
    EXPECT_EQ(st.size(), 3);
    int sum = 0;
    for (auto x : st)
      sum += x;
    EXPECT_EQ(sum, 6);
  }
  {
    unordered_set<int> a = {1, 2, 3}, b = {1, 2, 3}, c = {1, 2, 4};
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    swap(a, c);
    EXPECT_EQ(c, b);
  }
  {
    unordered_set<int> a = {1, 2, 3, 4, 5, 6}, b = {2, 4, 6};
    erase_if(a, [](auto &key) { return key % 2 == 1; });
    EXPECT_EQ(a, b);
  }
}

TEST(test_unordered_map_set, node_handle) {
  {
    unordered_set<int> a = {1, 2, 3}, b = {1, 3}, c = a;
    auto nh = a.extract(2);
    EXPECT_EQ(nh.value(), 2);
    EXPECT_EQ(b, a);
    b.insert(move(nh));
    EXPECT_EQ(b, c);
  }
  {
    unordered_map<int, int> a = {{1, 10}, {2, 20}, {3, 30}}, b = {{1, 10}, {3, 30}}, c = a;
    auto nh = a.extract(2);
    EXPECT_EQ(nh.key(), 2);
    EXPECT_EQ(nh.mapped(), 20);
    EXPECT_EQ(b, a);
    b.insert(move(nh));
    EXPECT_EQ(b, c);
  }
}

TEST(test_unordered_map_set, emplace) {
  {
    unordered_map<int, int> a = {{1, 10}, {2, 20}, {3, 30}}, b;
    b.emplace(1, 10);
    b.emplace(2, 20);
    b.emplace(3, 30);
    EXPECT_EQ(b, a);
  }
  {
    unordered_set<int> a = {1, 2, 3}, b;
    b.emplace(1);
    b.emplace(2);
    b.emplace(3);
    EXPECT_EQ(b, a);
  }
}