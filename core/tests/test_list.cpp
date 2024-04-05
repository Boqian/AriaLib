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
}
