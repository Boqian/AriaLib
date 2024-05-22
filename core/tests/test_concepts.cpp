#include "concepts.h"
#include "gtest/gtest.h"

void test_concepts() {
  using namespace aria;
  static_assert(same_as<int, int>);
  static_assert(!same_as<int, double>);

  struct Base {};
  struct Derived : Base {
    int x;
  };

  static_assert(convertible_to<int, double>);
  static_assert(!convertible_to<int, Base>);
  static_assert(convertible_to<Derived, Base>);
  static_assert(!convertible_to<Base, Derived>);

  static_assert(signed_integral<int>);
  static_assert(unsigned_integral<unsigned int>);
}

TEST(test_concept, ranges_swap) {
  int a[3] = {1, 2};
  int b[3] = {4, 5};
  aria::ranges::swap(a, b);
  EXPECT_EQ(a[0], 4);
  EXPECT_EQ(a[1], 5);
  EXPECT_EQ(b[0], 1);
  EXPECT_EQ(b[1], 2);
}