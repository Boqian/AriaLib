#include "gtest/gtest.h"
#include "list.h"

using namespace aria;

TEST(test_list, base) {
  list<int> li;
  EXPECT_EQ(li.size(), 0);
  EXPECT_TRUE(li.empty());
  li.push_back(10);
  EXPECT_EQ(li.size(), 1);
  EXPECT_FALSE(li.empty());
  EXPECT_EQ(*li.begin(), 10);
  EXPECT_EQ(li.front(), 10);
  EXPECT_EQ(li.back(), 10);
  li.push_back(20);
  EXPECT_EQ(*li.begin(), 10);
  EXPECT_EQ(*(++li.begin()), 20);
  EXPECT_EQ(li.front(), 10);
  EXPECT_EQ(li.back(), 20);
  EXPECT_EQ(li.size(), 2);
  li.push_front(30);
  EXPECT_EQ(li.front(), 30);
  EXPECT_EQ(li.size(), 3);
  li.pop_back();
  EXPECT_EQ(li.size(), 2);
  EXPECT_EQ(li.front(), 30);
  EXPECT_EQ(li.back(), 10);
  li.pop_front();
  EXPECT_EQ(li.size(), 1);
  EXPECT_EQ(li.front(), 10);
  EXPECT_EQ(li.back(), 10);
  li.pop_front();
  EXPECT_TRUE(li.empty());
}

TEST(test_list, init) {
  list<int> li = {1, 2, 3};
  EXPECT_EQ(li.size(), 3);
  EXPECT_EQ(li.front(), 1);
  EXPECT_EQ(li.back(), 3);
  li.front() = 5;
  li.back() = 4;
  EXPECT_EQ(li, list<int>({5, 2, 4}));
  li.clear();
  EXPECT_EQ(li, list<int>());
}

TEST(test_list, reverse_iterator) {
  {
    list<int> v = {1, 2, 3}, u;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      u.push_back(*it);
    }
    EXPECT_EQ(u, list<int>({3, 2, 1}));
  }
  {
    list<pair<int, double>> v = {{3, 1.5}, {2, 4.4}};
    EXPECT_EQ(v.rbegin()->first, 2);
    EXPECT_DOUBLE_EQ(v.rbegin()->second, 4.4);
    v.rbegin()->first = 10;
    EXPECT_EQ(v.back().first, 10);
    *v.rbegin() = pair(11, 5.5);
    EXPECT_EQ(v.rbegin()->first, 11);
  }
}