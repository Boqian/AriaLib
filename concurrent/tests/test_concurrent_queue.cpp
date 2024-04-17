#include "concurrent_queue.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

using namespace aria;

void producer(concurrent_queue<int> &q, int n) {
  for (int i = 0; i < n; i++) {
    q.push(i);
  }
}

std::vector<int> consumer(concurrent_queue<int> &q, int n) {
  std::vector<int> res;
  while (n--) {
    res.push_back(q.pop());
  }
  return res;
}

TEST(test_concurrent_queue, basic) {
  const int n = 1000;
  concurrent_queue<int> q;
  std::packaged_task task([&] { return consumer(q, n); });
  auto fut = task.get_future();
  auto j1 = std::jthread([&] { producer(q, n); });
  auto j2 = std::jthread(std::move(task));
  auto result = fut.get();

  std::vector<int> expected(n);
  std::iota(expected.begin(), expected.end(), 0);

  EXPECT_EQ(result, expected);
}