#include "allocator.h"
#include "gtest/gtest.h"
#include <utility>

using namespace aria;

TEST(test_allocator, base) {
  allocator<int> alloc;
  int n = 5;
  auto p = alloc.allocate(n);
  for (int i = 0; i < n; i++) {
    *(p+i) = 10+i;
    EXPECT_EQ(*(p + i), 10+i);
  }
  alloc.deallocate(p, n);
}

    