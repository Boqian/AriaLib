#include "../thread_pool.h"
#include "gtest/gtest.h"
#include <set>
#include <thread>

using namespace aria;

TEST(test_thread_pool, basic) { ThreadPool tp(4);
  Synchronized<std::set<int>> st;
  EXPECT_FALSE(tp.is_running());
  tp.start();
  auto f = [&](int i) { st.wlock()->insert(i); };
  for (int i = 0; i < 5; i++) {
    tp.detach_task([&, i] { f(i); });
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto result = *st.rlock();
  EXPECT_EQ(result, std::set<int>({0, 1, 2, 3, 4}));

  tp.stop();
}