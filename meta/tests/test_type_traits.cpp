#include "../type_traits.h"
#include <string>

void test_traits() {
  using namespace aria;
  static_assert(is_same_v<conditional_t<true, int, double>, int>);
  static_assert(is_same_v<conditional_t<false, int, double>, double>);

  static_assert(is_same_v<int, int>);
  static_assert(!is_same_v<int, double>);

  static_assert(is_convertible_v<int, int>);
  static_assert(is_convertible_v<int, double>);
  static_assert(!is_convertible_v<int, std::string>);
  static_assert(!is_convertible_v<std::string, int>);
}