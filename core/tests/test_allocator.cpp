#include "allocator.h"
#include "gtest/gtest.h"
#include <utility>

using namespace aria;

struct A {
  static inline int n_ctor = 0;
  static inline int n_dtor = 0;
  static void reset() {
    n_ctor = 0;
    n_dtor = 0;
  }
  A() { n_ctor++; }
  ~A() { n_dtor++; }
};

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

TEST(test_allocator, construct) {
  A::reset();
  allocator<A> alloc;
  int n = 5;
  auto p = alloc.allocate(n);
  EXPECT_EQ(A::n_ctor, 0);
  EXPECT_EQ(A::n_dtor, 0);
  alloc.deallocate(p, n);
  EXPECT_EQ(A::n_ctor, 0);
  EXPECT_EQ(A::n_dtor, 0);

  A::reset();
  p = alloc.allocate(n);
  aria::construct_at(p);
  EXPECT_EQ(A::n_ctor, 1);
  aria::construct_at(p + 1);
  EXPECT_EQ(A::n_ctor, 2);
  EXPECT_EQ(A::n_dtor, 0);
  aria::destroy_at(p);
  EXPECT_EQ(A::n_dtor, 1);
  aria::destroy_at(p + 1);
  EXPECT_EQ(A::n_dtor, 2);
  alloc.deallocate(p, n);
  EXPECT_EQ(A::n_dtor, 2);
}
    