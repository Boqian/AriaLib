#include "priority_queue.h"
#include "gtest/gtest.h"
#include <algorithm>
using namespace aria;

TEST(test_priority_queue, basic) {
  vector<int> v = {9, 6, 9, 5, 5, 9, 7, 1, 1, 3, 5, 8, 3, 4, 2}, u = v;
  std::sort(u.begin(), u.end(), greater());
  priority_queue<int> q(v.begin(), v.end());
  vector<int> sorted;
  while (!q.empty()) {
    sorted.push_back(q.top());
    q.pop();
  }
  EXPECT_EQ(sorted, u);
}
