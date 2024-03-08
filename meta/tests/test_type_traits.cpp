#include "../type_traits.h"


void test_traits() {
  static_assert(aria::is_same_v<int, int>);
  static_assert(!aria::is_same_v<int, double>);
}