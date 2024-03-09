#include "../concepts.h"

void test_concepts() {
  static_assert(aria::same_as<int, int>);
  static_assert(!aria::same_as<int, double>);



}