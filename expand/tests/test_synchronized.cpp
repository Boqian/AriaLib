#include "../synchronized.h"
#include "gtest/gtest.h"
#include <vector>
#include <queue>
#include <thread>

using namespace aria;

TEST(test_synchronized, shared_mutex_basic) {
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

TEST(test_synchronized, mutex_basic) {
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

TEST(test_synchronized, try_exclusive_lock) {
  auto q = Synchronized<int, std::mutex>();
  auto lock1 = q.lock();
  EXPECT_TRUE(lock1.is_locked());

  auto t = std::jthread([&] {
    auto lock2 = q.try_lock();
    EXPECT_FALSE(lock2.is_locked());
  });
}

TEST(test_synchronized, try_shared_lock) {
  auto q = Synchronized<std::queue<int>>();
  auto lock1 = q.rlock();
  EXPECT_TRUE(lock1.is_locked());

  auto t = std::jthread([&] {
    auto lock2 = q.try_rlock();
    EXPECT_TRUE(lock2.is_locked());
  });
}