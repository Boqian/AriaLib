#include "list.h"
#include "ranges.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(ranges::range<vector<int>>);

TEST(test_ranges, begin_end_size) {
  {
    int x[10]{};
    EXPECT_EQ(ranges::size(x), 10);

    auto it = ranges::begin(x);
    static_assert(is_same_v<decltype(it), int *>);
    *it = 5;
    EXPECT_EQ(x[0], 5);

    auto it2 = ranges::end(x);
    static_assert(is_same_v<decltype(it2), int *>);
    EXPECT_EQ(x + 10, it2);
  }
  {
    list<int> li = {1, 2, 3, 4};
    EXPECT_EQ(ranges::size(li), 4);
  }
}