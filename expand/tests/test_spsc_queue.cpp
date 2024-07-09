#include "spsc_queue.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

using namespace aria;

TEST(test_spsc_queue, basic) {
  const int n = 100;

  spsc_queue<int> q(n);
}