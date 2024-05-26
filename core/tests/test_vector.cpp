#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(random_access_iterator<vector<int>::iterator>);
static_assert(random_access_iterator<vector<int>::const_iterator>);

struct A {
  static inline int n_ctor = 0;
  static inline int n_dtor = 0;
  static void reset() {
    n_ctor = 0;
    n_dtor = 0;
  }
  A() { n_ctor++; }
  A(const A &) { n_ctor++; }
  A(A &&) = default;
  ~A() { n_dtor++; }
};

TEST(test_vector, basic) {
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
  {
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
  {
    vector<vector<int>> v = {{1, 2}, {2, 3}};
    EXPECT_EQ(v[0], vector<int>({1, 2}));
    EXPECT_EQ(v[1], vector<int>({2, 3}));
  }
  {
    vector<pair<int, int>> v = {{1, 2}, {2, 3}};
    EXPECT_EQ(v[0], pair(1, 2));
    EXPECT_EQ(v[1], pair(2, 3));
  }
}

TEST(test_vector, emplace_back) {
  auto v = vector<pair<int, double>>();
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

TEST(test_vector, assign) {
  auto v = vector<int>({1, 2, 3});
  auto a = vector<int>({1}), b = a;
  a = v;
  EXPECT_EQ(a, v);
  b = move(v);
  EXPECT_EQ(a, b);
  EXPECT_EQ(v, vector<int>());
}

TEST(test_vector, ctor_and_dtor) {
  A::reset();
  int n = 10;
  {
    auto v = vector<A>();
    v.reserve(10);
    for (int i = 0; i < n; i++) {
      v.emplace_back();
      EXPECT_EQ(A::n_ctor, i + 1);
      EXPECT_EQ(A::n_dtor, 0);
    }
  }
  EXPECT_EQ(A::n_dtor, n);
}

TEST(test_vector, ctor_and_dtor_copy) {
  int n = 10;
  A a;
  A::reset();
  {
    auto v = vector<A>(n, a);
    EXPECT_EQ(A::n_ctor, n);
    EXPECT_EQ(A::n_dtor, 0);
    auto u = move(v);
    EXPECT_EQ(A::n_ctor, n);
    EXPECT_EQ(A::n_dtor, 0);
    auto w = u;
    EXPECT_EQ(A::n_ctor, n * 2);
    EXPECT_EQ(A::n_dtor, 0);
  }
  EXPECT_EQ(A::n_dtor, n * 2);
}

TEST(test_vector, iterator) {
  {
    vector<int> v = {1, 2, 3};
    auto it = v.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    it += 2;
    EXPECT_EQ(it, v.end());
    --it;
    EXPECT_EQ(*it, 3);
    it -= 2;
    EXPECT_EQ(it, v.begin());
    *it = 5;
    EXPECT_EQ(v[0], 5);
  }
  {
    vector<int> v = {1, 2, 3};
    for (auto &x : v) {
      x++;
    }
    EXPECT_EQ(v, vector<int>({2, 3, 4}));
  }
  {
    const vector<int> v = {1, 2, 3};
    vector<int> u;
    for (auto it = v.begin(); it != v.end(); ++it) {
      u.push_back(*it * 2);
    }
    EXPECT_EQ(u, vector<int>({2, 4, 6}));
  }
  {
    auto v = vector<pair<int, double>>();
    v.emplace_back(3, 1.5);
    EXPECT_EQ(v.begin()->first, 3);
    EXPECT_DOUBLE_EQ(v.begin()->second, 1.5);
    *v.begin() = pair(6, 1.0);
    EXPECT_EQ(v[0].first, 6);
  }
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(v.begin() + 4, v.end());
    EXPECT_EQ(v.begin() += 4, v.end());
    EXPECT_EQ(v.end() - 4, v.begin());
    EXPECT_EQ(v.end() -= 4, v.begin());
  }
}

TEST(test_vector, reverse_iterator) {
  {
    vector<int> v = {1, 2, 3}, u;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      u.push_back(*it);
    }
    EXPECT_EQ(u, vector<int>({3, 2, 1}));
  }
  {
    vector<pair<int, double>> v = {{3, 1.5}, {2, 4.4}};
    EXPECT_EQ(v.rbegin()->first, 2);
    EXPECT_DOUBLE_EQ(v.rbegin()->second, 4.4);
    v.rbegin()->first = 10;
    EXPECT_EQ(v[1].first, 10);
    *v.rbegin() = pair(11, 5.5);
    EXPECT_EQ(v.rbegin()->first, 11);
  }
}

TEST(test_vector, 2d_vector) {
  vector<vector<int>> vv = {{1, 2}, {3, 4}};
  EXPECT_EQ(vv[0][0], 1);
  EXPECT_EQ(vv[0][1], 2);
  EXPECT_EQ(vv[1][0], 3);
  EXPECT_EQ(vv[1][1], 4);
}

TEST(test_vector, shrink_to_fit) {
  vector<int> v = {1, 2, 3};
  v.reserve(100);
  EXPECT_EQ(v, vector<int>({1, 2, 3}));
  auto u = v;
  EXPECT_EQ(u, vector<int>({1, 2, 3}));
  EXPECT_EQ(v.capacity(), 100);
  EXPECT_EQ(u.capacity(), 3);
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), 3);
  EXPECT_EQ(v, vector<int>({1, 2, 3}));
}

TEST(test_vector, erase) {
  vector<int> v = {1, 2, 3};
  auto it = v.erase(begin(v) + 1);
  EXPECT_EQ(*it, 3);
  EXPECT_EQ(v, vector({1, 3}));
}