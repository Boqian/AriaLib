#include "algorithm.h"
#include "list.h"
#include "numeric.h"
#include "vector.h"
#include <algorithm>

#include "gtest/gtest.h"

using namespace aria;

TEST(test_algorithm, for_each) {
  {
    vector<int> v = {1, 2, 3};
    for_each(v.begin(), v.end(), [](int &x) { x *= 2; });
    EXPECT_EQ(v, vector({2, 4, 6}));
  }
  {
    vector<int> v = {1, 2, 3};
    for_each_n(v.begin(), 3, [](int &x) { x *= 2; });
    EXPECT_EQ(v, vector({2, 4, 6}));
  }
}

TEST(test_algorithm, all_of) {
  const vector<int> v = {1, 2, 3, 4, 5};
  EXPECT_TRUE(all_of(v.begin(), v.end(), [](int x) { return x <= 5; }));
  EXPECT_FALSE(all_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
  EXPECT_FALSE(none_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
  EXPECT_TRUE(any_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
  EXPECT_TRUE(none_of(v.begin(), v.end(), [](int x) { return x > 5; }));
}

TEST(test_algorithm, count) {
  const vector<int> v = {1, 2, 3, 4, 2, 5, 2};
  EXPECT_EQ(count(v.begin(), v.end(), 2), 3);
  EXPECT_EQ(count(v.begin(), v.end(), 1), 1);
  EXPECT_EQ(count_if(v.begin(), v.end(), [](int x) { return x % 2 == 1; }), 3);
}

TEST(test_algorithm, reverse) {
  {
    vector<int> v = {1, 2, 3}, u = {3, 2, 1};
    reverse(v.begin(), v.end());
    EXPECT_EQ(v, u);
  }
  {
    list<int> v = {1, 2, 3}, u = {3, 2, 1};
    reverse(v.begin(), v.end());
    EXPECT_EQ(v, u);
  }
}

TEST(test_algorithm, find) {
  {
    vector<int> v = {1, 2, 3};
    auto it = aria::find(v.begin(), v.end(), 2);
    EXPECT_EQ(it, v.begin() + 1);
  }
  {
    const vector<int> v = {1, 2, 3};
    auto it = aria::find_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    EXPECT_EQ(it, v.begin() + 1);
  }
}

TEST(test_algorithm, minmax) {
  {
    vector<int> v = {4, 1, 3, 2};
    auto x = *min_element(v.begin(), v.end());
    EXPECT_EQ(x, 1);
    auto y = *min_element(v.begin(), v.end(), [](int a, int b) { return a > b; });
    EXPECT_EQ(y, 4);
  }
  {
    EXPECT_EQ(aria::min(1, 5), 1);
    EXPECT_EQ(aria::min({3, 4, 5}), 3);
  }
  {
    vector<int> v = {1, 4, 3, 2};
    auto x = *max_element(v.begin(), v.end());
    EXPECT_EQ(x, 4);
    auto y = *max_element(v.begin(), v.end(), [](int a, int b) { return a > b; });
    EXPECT_EQ(y, 1);
  }
  {
    EXPECT_EQ(aria::max(1, 5), 5);
    EXPECT_EQ(aria::max({3, 4, 5}), 5);
  }
}

TEST(test_algorithm, numeric) {
  {
    static_assert(gcd(16, 24) == 8);
    static_assert(lcm(16, 24) == 48);
    static_assert(gcd(-16, 24) == 8);
  }
  {
    vector<int> v = {4, 1, 3, 2};
    EXPECT_EQ(accumulate(v.begin(), v.end(), 0), 10);
    EXPECT_EQ(reduce(v.begin(), v.end(), 0), 10);
    EXPECT_EQ(reduce(v.begin(), v.end()), 10);
    auto op = [](int a, int b) { return a * b; };
    EXPECT_EQ(accumulate(v.begin(), v.end(), 1, op), 24);
    EXPECT_EQ(reduce(v.begin(), v.end(), 1, op), 24);
  }
}

TEST(test_algorithm, copy) {
  {
    vector<int> a = {1, 2, 3}, b(3, 0), c(3, 0), d(3, 0);
    copy(begin(a), end(a), begin(b));
    EXPECT_EQ(b, vector({1, 2, 3}));
    auto it = copy_if(begin(a), end(a), begin(c), [](int x) { return x % 2 == 1; });
    EXPECT_EQ(c, vector({1, 3, 0}));
    EXPECT_EQ(distance(begin(c), it), 2);
    copy_n(begin(a), 3, begin(d));
    EXPECT_EQ(d, vector({1, 2, 3}));
  }
}

TEST(test_algorithm, partition) {
  {
    auto is_even = [](auto x) { return x % 2 == 0; };
    vector<int> a = {1, 2, 3, 4, 5, 6}, b = {2, 4, 6, 1, 3, 5};
    EXPECT_TRUE(is_partitioned(begin(b), end(b), is_even));
    EXPECT_FALSE(is_partitioned(begin(a), end(a), is_even));

    auto it = partition(begin(a), end(a), is_even);
    EXPECT_TRUE(is_partitioned(begin(a), end(a), is_even));
    EXPECT_EQ(distance(begin(a), it), 3);

    auto it2 = partition_point(begin(a), end(a), is_even);
    EXPECT_EQ(it2, it);
  }
}

TEST(test_algorithm, lower_upper_bound) {
  {
    vector<int> a = {1, 2, 3, 4, 6, 7};
    auto it = lower_bound(begin(a), end(a), 4);
    EXPECT_EQ(*it, 4);
    it = lower_bound(begin(a), end(a), 5);
    EXPECT_EQ(*it, 6);
  }
  {
    vector<int> a = {1, 2, 3, 4, 6, 7};
    auto it = upper_bound(begin(a), end(a), 4);
    EXPECT_EQ(*it, 6);
    it = upper_bound(begin(a), end(a), 5);
    EXPECT_EQ(*it, 6);
  }
  {
    vector<int> a = {1, 2, 3, 3, 4, 6, 7};
    auto [l, r] = equal_range(begin(a), end(a), 3);
    EXPECT_EQ(distance(begin(a), l), 2);
    EXPECT_EQ(distance(begin(a), r), 4);
  }
  {
    vector<int> a = {1, 2, 3, 3, 4, 6, 7};
    EXPECT_TRUE(binary_search(begin(a), end(a), 4));
    EXPECT_FALSE(binary_search(begin(a), end(a), 5));
  }
}

TEST(test_algorithm, heap) {
  {
    vector<int> v{9, 6, 9, 5, 5, 9, 7, 1, 1, 3, 5, 8, 3, 4, 2};
    EXPECT_TRUE(is_heap(v.begin(), v.end()));
  }
  {
    vector<int> v{1, 2, 3};
    EXPECT_FALSE(is_heap(v.begin(), v.end()));
    make_heap(v.begin(), v.end());
    EXPECT_EQ(v, (vector{3, 2, 1}));
  }
  {
    vector<int> v{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
    EXPECT_FALSE(is_heap(v.begin(), v.end()));
    make_heap(v.begin(), v.end());
    EXPECT_TRUE(is_heap(v.begin(), v.end()));
    EXPECT_TRUE(std::is_heap(v.begin(), v.end()));
  }
  {
    vector<int> v{3, 2, 1};
    v.push_back(4);
    EXPECT_FALSE(is_heap(v.begin(), v.end()));
    push_heap(v.begin(), v.end());
    EXPECT_EQ(v, (vector{4, 3, 1, 2}));
    EXPECT_TRUE(is_heap(v.begin(), v.end()));
    EXPECT_TRUE(std::is_heap(v.begin(), v.end()));
  }
  {
    vector<int> v{9, 6, 9, 5, 5, 9, 7, 1, 1, 3, 5, 8, 3, 4, 2};
    EXPECT_TRUE(is_heap(v.begin(), v.end()));
    while (!v.empty()) {
      pop_heap(v.begin(), v.end());
      v.pop_back();
      EXPECT_TRUE(is_heap(v.begin(), v.end()));
      EXPECT_TRUE(std::is_heap(v.begin(), v.end()));
    }
  }
  {
    vector<int> v{9, 6, 9, 5, 5, 9, 7, 1, 1, 3, 5, 8, 3, 4, 2};
    EXPECT_TRUE(is_heap(v.begin(), v.end()));
    sort_heap(v.begin(), v.end());
    EXPECT_TRUE(is_sorted(v.begin(), v.end()));
  }
}

TEST(test_algorithm, sort) {
  {
    vector<int> a = {1, 4, 7, 2, 5, 8, 3, 6, 9}, b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    insertion_sort(a.begin(), a.end());
  }
  {
    vector<int> a = {1, 2, 3}, b = {2, 1, 3}, c = {2, 2, 3};
    EXPECT_TRUE(is_sorted(a.begin(), a.end()));
    EXPECT_FALSE(is_sorted(b.begin(), b.end()));
    EXPECT_TRUE(is_sorted(c.begin(), c.end()));
  }
  {
    vector<int> a = {3, 2, 1}, b = {1, 2, 3};
    sort(a.begin(), a.end());
    EXPECT_EQ(a, b);
  }
  {
    vector<int> a = {1, 4, 7, 2, 5, 8, 3, 6, 9}, b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    sort(a.begin(), a.end());
    EXPECT_EQ(a, b);
  }
  {
    const int n = 10000;
    vector<int> v;
    for (int i = 0; i < n; i += 2) {
      v.push_back(n - i);
      v.push_back(i + 1);
    }
    auto u = v;
    std::sort(u.begin(), u.end());
    sort(v.begin(), v.end());
    EXPECT_EQ(u, v);
  }
}