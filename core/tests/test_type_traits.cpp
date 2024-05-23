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
  static_assert(is_same_v<int &, add_lvalue_reference_t<int>>);

  static_assert(!is_base_of_v<int, double>);
  static_assert(is_base_of_v<Base, Derived>);
  static_assert(!is_base_of_v<Base, void>);
  static_assert(!is_base_of_v<void, Derived>);
  static_assert(!is_base_of_v<int, short>);
  static_assert(!is_base_of_v<int, const int>);

  static_assert(is_same_v<int, remove_reference_t<int>>);
  static_assert(is_same_v<int, remove_reference_t<int &>>);
  static_assert(is_same_v<int, remove_reference_t<int &&>>);

  static_assert(is_same_v<int, remove_cvref_t<const int &>>);
  static_assert(is_same_v<int, remove_cvref_t<int &&>>);
  static_assert(is_same_v<int, remove_cvref_t<const int>>);

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

  static_assert(is_same_v<void, common_type_t<void>>);
  static_assert(is_same_v<int, common_type_t<int, short>>);
  static_assert(is_same_v<int, common_type_t<int, short, char>>);
  static_assert(is_same_v<long long, common_type_t<int, short, char, long long>>);

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

  static_assert(is_same_v<add_const_t<int>, const int>);
  static_assert(is_same_v<remove_const_t<int>, int>);
  static_assert(is_same_v<remove_const_t<const int>, int>);

  static_assert(is_same_v<remove_cvref_t<const int>, int>);
  static_assert(is_same_v<remove_cvref_t<const int &>, int>);
  static_assert(is_same_v<remove_cvref_t<int &>, int>);

  static_assert(!is_copy_contructible_v<CannotCopy>);
  static_assert(is_copy_contructible_v<AA>);
  static_assert(is_copy_contructible_v<int>);
  static_assert(is_move_contructible_v<int>);
  static_assert(is_move_contructible_v<AA>);
  static_assert(is_move_contructible_v<CannotCopy>);
  static_assert(is_copy_contructible_v<CannotMove>);
  static_assert(!is_move_contructible_v<CannotMove>);

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
