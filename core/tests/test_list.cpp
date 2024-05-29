#include "list.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(bidirectional_iterator<list<int>::iterator>);
static_assert(bidirectional_iterator<list<int>::const_iterator>);

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

TEST(test_list, iterator) {
  {
    list<int> v;
    for (auto it = v.begin(); it != v.end(); ++it) {
      EXPECT_EQ(*it, 12345);
    }
    for (auto x : v) {
      EXPECT_EQ(x, 12345);
    }
  }
  {
    list<int> v = {1, 2, 3, 4};
    auto dis = distance(v.begin(), v.end());
    EXPECT_EQ(dis, 4);
  }
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

TEST(test_list, insert_erase) {
  {
    list<int> v = {1, 2, 3}, u;
    auto pos = ++v.begin();
    EXPECT_EQ(*pos, 2);
    auto it = v.insert(pos, 10);
    EXPECT_EQ(*it, 10);
    EXPECT_EQ(v, list<int>({1, 10, 2, 3}));
    it = v.erase(it);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(v, list<int>({1, 2, 3}));
    it = v.erase(it);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(v, list<int>({1, 3}));
    it = v.erase(it);
    EXPECT_EQ(it, v.end());
    EXPECT_EQ(v, list<int>({1}));
  }
}

void test_iterate(const list<int> &v) {
  int count = 0, sum = 0;
  for (auto x : v) {
    count++;
    sum += x;
  }
  EXPECT_EQ(count, v.size());
  count = 0;
  for (auto it = v.rbegin(); it != v.rend(); ++it) {
    count++;
    sum += *it;
  }
  EXPECT_EQ(count, v.size());
}

TEST(test_list, construct) {
  {
    list<int> v = {1, 2, 3};
    auto u = v;
    EXPECT_EQ(u, list<int>({1, 2, 3}));
  }
  {
    list<int> v = {1, 2, 3}, u = {4, 5};
    u = v;
    EXPECT_EQ(u, list<int>({1, 2, 3}));
  }
  {
    list<int> v = {1, 2, 3};
    auto u = aria::move(v);
    EXPECT_EQ(u, list<int>({1, 2, 3}));
    EXPECT_TRUE(v.empty());
  }
  {
    list<int> v = {1, 2, 3}, u = {4, 5};
    u = move(v);
    EXPECT_EQ(u, list<int>({1, 2, 3}));
    EXPECT_TRUE(v.empty());
  }
  {
    list<int> v;
    auto u = move(v);
    test_iterate(v);
    test_iterate(u);
  }
  {
    list<int> v = {1, 2, 3};
    auto u = move(v);
    test_iterate(v);
    test_iterate(u);
  }
}

TEST(test_list, reverse) {
  list<int> v = {1, 2, 3};
  auto it = begin(v);
  v.reverse();
  EXPECT_EQ(v, list<int>({3, 2, 1}));
  EXPECT_EQ(*it, 1);
  EXPECT_EQ(*prev(it), 2);
}

TEST(test_list, merge) {
  {
    list<int> a, b;
    a.merge(b);
    EXPECT_EQ(a, list<int>());
    EXPECT_EQ(b, list<int>());
  }
  {
    list<int> a = {1}, b = {1};
    a.merge(b);
    EXPECT_EQ(a, list<int>({1, 1}));
    EXPECT_EQ(b, list<int>());
  }
  {
    list<int> a = {1}, b = {};
    a.merge(b);
    EXPECT_EQ(a, list<int>({1}));
    EXPECT_EQ(b, list<int>());
  }
  {
    list<int> a = {}, b = {1};
    a.merge(b);
    EXPECT_EQ(a, list<int>({1}));
    EXPECT_EQ(b, list<int>());
  }
  {
    list<int> a = {1, 3, 5}, b = {2, 4};
    a.merge(b);
    EXPECT_EQ(a, list<int>({1, 2, 3, 4, 5}));
    EXPECT_EQ(b, list<int>());
  }
  {
    list<int> a = {1, 3}, b = {1, 4};
    a.merge(b);
    EXPECT_EQ(a, list<int>({1, 1, 3, 4}));
    EXPECT_EQ(b, list<int>());
  }
}

TEST(test_list, sort) {
  {
    list<int> a;
    a.sort();
    EXPECT_EQ(a, list<int>());
  }
  {
    list<int> a = {5};
    a.sort();
    EXPECT_EQ(a, list<int>({5}));
  }
  {
    list<int> a = {6, 2};
    a.sort();
    EXPECT_EQ(a, list<int>({2, 6}));
  }
  {
    list<int> a = {2, 6, 4, 3, 5, 7, 1};
    a.sort();
    EXPECT_EQ(a, list<int>({1, 2, 3, 4, 5, 6, 7}));
  }
}

TEST(test_list, unique) {
  {
    list<int> a;
    a.unique();
    EXPECT_EQ(a, list<int>());
  }
  {
    list<int> a = {5};
    a.unique();
    EXPECT_EQ(a, list<int>({5}));
  }
  {
    list<int> a = {5, 5};
    a.unique();
    EXPECT_EQ(a, list<int>({5}));
  }
  {
    list<int> a = {2, 6, 6, 6, 2, 2, 5, 5, 5, 7, 1, 1};
    a.unique();
    EXPECT_EQ(a, list<int>({2, 6, 2, 5, 7, 1}));
  }
}

TEST(test_list, remove_erase) {
  {
    list<int> a = {2, 6, 6, 6, 2, 2, 5, 5, 5, 7, 1, 1}, b = a;
    a.remove(2);
    EXPECT_EQ(a, list<int>({6, 6, 6, 5, 5, 5, 7, 1, 1}));
    b.remove_if([](int x) { return x % 2 == 0; });
    EXPECT_EQ(b, list<int>({5, 5, 5, 7, 1, 1}));
  }
  {
    list<int> a = {2, 6, 6, 6, 2, 2, 5, 5, 5, 7, 1, 1}, b = a;
    erase(a, 2);
    EXPECT_EQ(a, list<int>({6, 6, 6, 5, 5, 5, 7, 1, 1}));
    erase_if(b, [](int x) { return x % 2 == 0; });
    EXPECT_EQ(b, list<int>({5, 5, 5, 7, 1, 1}));
  }
}