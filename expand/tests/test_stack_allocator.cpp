#include "stack_allocator.h"
#include "string.h"
#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_stack_allocator, basic) {
  StackBuffer buf;
  StackAllocator<int, buf.size()> alloc(buf);
  vector<int, decltype(alloc)> v(alloc);
  for (int i = 0; i < 100; i++)
    v.push_back(i);
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(v[i], i);
  }
}