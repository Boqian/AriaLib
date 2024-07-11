#include "mystring.h"
#include "type_traits.h"
#include "utility.h"
#include "gtest/gtest.h"

struct Base {};
struct Derived : Base {};
struct AA {
  explicit AA(int) {}
};

struct BB {
  ~BB() = delete;
};

struct CC {
  CC(int, double) {}
};

struct ThrowDtor {
  ~ThrowDtor() noexcept(false) {}
};

struct NoThrowDtor {
  ~NoThrowDtor() {} // noexcept by default
};

struct CannotCopy {
  CannotCopy(CannotCopy &){};
  CannotCopy(const CannotCopy &) = delete;
  CannotCopy(CannotCopy &&) = default;
};

struct CannotMove {
  CannotMove(CannotMove &&) = delete;
  CannotMove(const CannotMove &) = default;
};

struct NoSwap {
  NoSwap(const NoSwap &) = delete;
  NoSwap(NoSwap &&) = delete;
};

struct NoStdSwap {
  NoStdSwap(const NoStdSwap &) = delete;
  NoStdSwap(NoStdSwap &&) = delete;
};

void swap(NoStdSwap &, NoStdSwap &) noexcept {}

struct ThrowSwap {
  ThrowSwap(ThrowSwap &&) = delete;
};

void swap(ThrowSwap &, ThrowSwap &) {}

template <class T, class U> void assert_same() { static_assert(aria::is_same_v<T, U>); }

void test_traits() {
  using namespace aria;
  assert_same<conditional_t<true, int, double>, int>();
  assert_same<conditional_t<false, int, double>, double>();

  assert_same<int, int>();
  static_assert(!is_same_v<int, double>);

  static_assert(is_convertible_v<int, int>);
  static_assert(is_convertible_v<int, double>);
  static_assert(!is_convertible_v<int, string>);
  static_assert(!is_convertible_v<string, int>);

  assert_same<void, add_lvalue_reference_t<void>>();
  assert_same<int &, add_lvalue_reference_t<int>>();

  static_assert(!is_base_of_v<int, double>);
  static_assert(is_base_of_v<Base, Derived>);
  static_assert(!is_base_of_v<Base, void>);
  static_assert(!is_base_of_v<void, Derived>);
  static_assert(!is_base_of_v<int, short>);
  static_assert(!is_base_of_v<int, const int>);

  assert_same<int, remove_reference_t<int>>();
  assert_same<int, remove_reference_t<int &>>();
  assert_same<int, remove_reference_t<int &&>>();

  assert_same<int, remove_cvref_t<const int &>>();
  assert_same<int, remove_cvref_t<int &&>>();
  assert_same<int, remove_cvref_t<const int>>();

  static_assert(!is_lvalue_reference_v<int>);
  static_assert(is_lvalue_reference_v<int &>);
  static_assert(!is_lvalue_reference_v<int &&>);

  static_assert(!is_rvalue_reference_v<int>);
  static_assert(!is_rvalue_reference_v<int &>);
  static_assert(is_rvalue_reference_v<int &&>);

  static_assert(is_default_constructible_v<int>);
  static_assert(is_default_constructible_v<Base>);
  static_assert(!is_default_constructible_v<AA>);

  static_assert(!is_constructible_v<CC, int>);
  static_assert(is_constructible_v<CC, int, double>);
  static_assert(is_constructible_v<AA, int>);
  static_assert(!is_constructible_v<AA>);
  static_assert(is_constructible_v<Base>);

  static_assert(is_destructible_v<int>);
  static_assert(!is_destructible_v<void>);
  static_assert(!is_destructible_v<BB>);
  static_assert(is_destructible_v<BB &>);
  static_assert(is_nothrow_destructible_v<int>);
  static_assert(is_nothrow_destructible_v<NoThrowDtor>);
  static_assert(!is_nothrow_destructible_v<ThrowDtor>);

  static_assert(is_integral_v<int>);
  static_assert(is_integral_v<char>);
  static_assert(is_integral_v<long>);
  static_assert(!is_integral_v<double>);
  static_assert(!is_integral_v<float>);

  static_assert(!is_floating_point_v<int>);
  static_assert(!is_floating_point_v<char>);
  static_assert(!is_floating_point_v<long>);
  static_assert(is_floating_point_v<double>);
  static_assert(is_floating_point_v<float>);

  assert_same<void, common_type_t<void>>();
  assert_same<int, common_type_t<int, short>>();
  assert_same<int, common_type_t<int, short, char>>();
  assert_same<long long, common_type_t<int, short, char, long long>>();

  static_assert(!is_const_v<int>);
  static_assert(is_const_v<const int>);

  static_assert(is_arithmetic_v<float>);
  static_assert(!is_arithmetic_v<BB>);

  static_assert(is_signed_v<int>);
  static_assert(!is_signed_v<unsigned int>);
  static_assert(!is_signed_v<BB>);
  static_assert(!is_unsigned_v<int>);
  static_assert(is_unsigned_v<unsigned int>);
  static_assert(!is_unsigned_v<BB>);

  assert_same<add_const_t<int>, const int>();
  assert_same<remove_const_t<int>, int>();
  assert_same<remove_const_t<const int>, int>();

  assert_same<remove_cvref_t<const int>, int>();
  assert_same<remove_cvref_t<const int &>, int>();
  assert_same<remove_cvref_t<int &>, int>();

  static_assert(!is_copy_constructible_v<CannotCopy>);
  static_assert(is_copy_constructible_v<AA>);
  static_assert(is_copy_constructible_v<int>);
  static_assert(is_move_constructible_v<int>);
  static_assert(is_move_constructible_v<AA>);
  static_assert(is_move_constructible_v<CannotCopy>);
  static_assert(is_copy_constructible_v<CannotMove>);
  static_assert(!is_move_constructible_v<CannotMove>);

  static_assert(is_assignable_v<int, int>);
  static_assert(is_assignable_v<int, int &>);
  static_assert(is_copy_assignable_v<int>);
  static_assert(is_move_assignable_v<int>);

  static_assert(!is_swappable_with_v<int, int>);
  static_assert(is_swappable_with_v<int &, int &>);
  static_assert(is_swappable_v<int>);
  static_assert(is_swappable_v<AA>);
  static_assert(!is_swappable_v<NoSwap>);
  static_assert(is_swappable_v<NoStdSwap>);
  static_assert(is_nothrow_swappable_v<NoStdSwap>);
  static_assert(is_nothrow_swappable_v<int>);
  static_assert(!is_nothrow_swappable_v<ThrowSwap>);
  static_assert(!is_nothrow_swappable_v<NoSwap>);

  static_assert(is_class_v<AA>);
  static_assert(!is_class_v<int>);

  assert_same<add_pointer_t<int>, int *>();
  assert_same<add_pointer_t<int &>, int *>();
  assert_same<remove_pointer_t<int *>, int>();
  assert_same<remove_pointer_t<int *const>, int>();

  assert_same<make_signed_t<unsigned int>, int>();
  assert_same<make_unsigned_t<unsigned int>, unsigned int>();
  assert_same<make_signed_t<int>, int>();
  assert_same<make_unsigned_t<int>, unsigned int>();

  static_assert(conjunction_v<>);
  static_assert(conjunction_v<true_type, true_type, true_type>);

  assert_same<remove_extent_t<int[5]>, int>();
  assert_same<remove_all_extents_t<int[5]>, int>();
  assert_same<remove_all_extents_t<int[5][5]>, int>();

  assert_same<decay_t<const int &>, int>();
  assert_same<decay_t<int[5]>, int *>();
}

TEST(test_type_traits, swap) {
  int a[3] = {1, 2};
  int b[3] = {4, 5};
  aria::swap(a, b);
  EXPECT_EQ(a[0], 4);
  EXPECT_EQ(a[1], 5);
  EXPECT_EQ(b[0], 1);
  EXPECT_EQ(b[1], 2);
}

void foo(int) {}

struct S1 {
  int x;
  int foo(int) {}
};

void test_function_class() {
  using namespace aria;
  static_assert(is_function_v<decltype(foo)>);
  static_assert(!is_function_v<int>);

  static_assert(is_member_object_pointer_v<decltype(&S1::x)>);
  static_assert(!is_member_function_pointer_v<decltype(&S1::x)>);
  static_assert(is_member_pointer_v<decltype(&S1::x)>);
  static_assert(is_member_function_pointer_v<decltype(&S1::foo)>);
  static_assert(!is_member_object_pointer_v<decltype(&S1::foo)>);
  static_assert(is_member_pointer_v<decltype(&S1::foo)>);
  static_assert(!is_member_pointer_v<int>);
}

void f0() {}
void f1(int) {}
int f2(int &) { return 1; }
int f3(int, double &) { return 1; }

void test_invocable() {
  using namespace aria;
  static_assert(is_invocable_v<decltype(f0)>);
  static_assert(!is_invocable_v<decltype(f0), int>);
  static_assert(is_invocable_v<decltype(f1), int>);
  static_assert(is_invocable_v<decltype(f1), int &>);
  static_assert(is_invocable_v<decltype(f2), int &>);
  static_assert(!is_invocable_v<decltype(f2), int>);
  static_assert(is_invocable_v<decltype(f3), const int, double &>);

  static_assert(is_invocable_r_v<decltype(f0), void>);
  static_assert(is_invocable_r_v<decltype(f1), void, int>);
  static_assert(is_invocable_r_v<decltype(f2), int, int &>);

  assert_same<invoke_result_t<decltype(f0)>, void>();
  assert_same<invoke_result_t<decltype(f1), int>, void>();
  assert_same<invoke_result_t<decltype(f2), int &>, int>();
}