#include "concepts.h"

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