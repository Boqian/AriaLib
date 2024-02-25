#include "../synchronized.h"
#include "gtest/gtest.h"
#include <vector>

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
