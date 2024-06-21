#include "algorithm.h"
#include "list.h"
#include "numeric.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

namespace {
using I = vector<int>::iterator;
static_assert(is_lvalue_reference_v<iter_reference_t<I>>);
static_assert(same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>>);
static_assert(same_as<iter_value_t<I>, int>);
static_assert(contiguous_iterator<I>);
} // namespace

TEST(test_iterator, distance) {
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(distance(v.begin(), v.end()), 4);
  }
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(advance(v.begin(), 4), v.end());
    EXPECT_EQ(advance(v.end(), -4), v.begin());
  }
  {
    list<int> v = {1, 2, 3, 4};
    EXPECT_EQ(advance(v.begin(), 4), v.end());
    EXPECT_EQ(advance(v.end(), -4), v.begin());
  }
  {
    vector<int> v = {1, 2, 3, 4};
    auto it = v.begin(), it2 = next(it), it3 = prev(it2);
    EXPECT_EQ(*it2, 2);
    EXPECT_EQ(it3, it);
  }
}

TEST(test_iterator, begin_end) {
  {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(begin(v), v.begin());
    EXPECT_EQ(end(v), v.end());
  }
  {
    const vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(begin(v), v.begin());
    EXPECT_EQ(end(v), v.end());
  }
  {
    const int v[3] = {1, 2, 3};
    EXPECT_EQ(*begin(v), 1);
    EXPECT_EQ(accumulate(begin(v), end(v), 0), 6);
    EXPECT_TRUE(all_of(begin(v), end(v), [](auto x) { return x > 0 && x < 4; }));
  }
}