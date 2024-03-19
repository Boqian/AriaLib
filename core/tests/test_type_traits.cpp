#include "../type_traits.h"
#include <string>

struct Base {};
struct Derived : Base {};

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

  static_assert(is_same_v<void, add_lvalue_reference_t<void>>);
  static_assert(is_same_v<int&, add_lvalue_reference_t<int>>);

  static_assert(!is_base_of_v<int, double>);
  static_assert(is_base_of_v<Base, Derived>);
  static_assert(!is_base_of_v<Base, void>);
  static_assert(!is_base_of_v<void, Derived>);
  static_assert(!is_base_of_v<int, short>);
  static_assert(!is_base_of_v<int, const int>);

  static_assert(is_same_v<int, remove_reference_t<int>>);
  static_assert(is_same_v<int, remove_reference_t<int&>>);
  static_assert(is_same_v<int, remove_reference_t<int&&>>);

  static_assert(!is_lvalue_reference_v<int>);
  static_assert(is_lvalue_reference_v<int &>);
  static_assert(!is_lvalue_reference_v<int &&>);

  static_assert(!is_rvalue_reference_v<int>);
  static_assert(!is_rvalue_reference_v<int &>);
  static_assert(is_rvalue_reference_v<int &&>);
}