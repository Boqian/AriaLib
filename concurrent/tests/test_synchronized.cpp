#include "../synchronized.h"
#include "gtest/gtest.h"
#include <vector>
#include <queue>

using namespace aria;

TEST(test_synchronized, vector) {
  auto v = Synchronized<std::vector<int>>();
  {
    auto wl = v.wlock();
    wl->push_back(124);
    EXPECT_EQ(wl->size(), 1);
  }
  { auto rl = v.rlock();
    EXPECT_EQ(rl->size(), 1);
  }
}

TEST(test_synchronized, mutex) {
  auto q = Synchronized<std::queue<int>, std::mutex>();
  {
    auto wl = q.lock();
    wl->push(124);
    EXPECT_EQ(wl->size(), 1);
  }
  {
    auto rl = q.lock();
    EXPECT_EQ(rl->size(), 1);
    EXPECT_EQ(rl->front(), 124);
  }
}