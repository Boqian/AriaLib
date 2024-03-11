#include <utility>

#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

struct Counter {
  static inline int n1 = 0;
  static inline int n2 = 0;
  static void init() {
    n1 = 0;
    n2 = 0;
  }
};

struct A {
  A() { Counter::n1++; }
  ~A() { Counter::n2++; }
};

TEST(test_vector, base) {
  vector<int> v(2, 100);
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(v[0], 100);
  EXPECT_EQ(v[1], 100);
  v.push_back(33);
  EXPECT_EQ(v[2], 33);
  EXPECT_EQ(v.size(), 3);
  v.pop_back();
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(v.at(1), 100);
}

TEST(test_vector, construct) {
  auto v = vector<int>({1, 2, 3});
  EXPECT_EQ(v.capacity(), 3);
  EXPECT_EQ(v.size(), 3);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);

  auto u = v;
  EXPECT_EQ(u.size(), 3);
  EXPECT_EQ(u[0], 1);
  EXPECT_EQ(u[1], 2);
  EXPECT_EQ(u[2], 3);
}

TEST(test_vector, emplace_back) {
  auto v = vector<std::pair<int, double>>();
  v.emplace_back(3, 1.5);
  EXPECT_EQ(v.size(), 1);
  EXPECT_EQ(v[0].first, 3);
  EXPECT_DOUBLE_EQ(v[0].second, 1.5);
}

TEST(test_vector, compare) {
  auto v = vector<int>({1, 2, 3});
  auto u = vector<int>({1, 2, 3});
  EXPECT_EQ(u, v);
  auto a = vector<int>({1, 2, 4});
  auto b = vector<int>({1, 2});
  EXPECT_NE(v, a);
  EXPECT_NE(v, b);
}