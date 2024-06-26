#include "../thread_pool.h"
#include "gtest/gtest.h"
#include <set>
#include <thread>

using namespace aria;

TEST(test_thread_pool, basic) {
  ThreadPool tp(4);
  Synchronized<std::set<int>> st;
  EXPECT_FALSE(tp.is_running());
  tp.start();
  auto f = [&](int i) { st.wlock()->insert(i); };
  for (int i = 0; i < 5; i++) {
    tp.detach_task([&, i] { f(i); });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto result = *st.rlock();
  EXPECT_EQ(result, std::set<int>({0, 1, 2, 3, 4}));

  tp.stop();
}

TEST(test_thread_pool, future) {
  ThreadPool tp(4);
  auto f1 = tp.submit_task([] { return 1; });
  auto f2 = tp.submit_task([] { return 2; });
  auto f3 = tp.submit_task([] { return 3; });
  tp.start();
  EXPECT_EQ(f1.get(), 1);
  EXPECT_EQ(f2.get(), 2);
  EXPECT_EQ(f3.get(), 3);
  tp.stop();
}

TEST(test_thread_pool, start_stop) {
  ThreadPool tp(4);
  tp.detach_task([] { });
  tp.start();
  tp.stop();
}