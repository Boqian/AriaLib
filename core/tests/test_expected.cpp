#include "expected.h"
#include "mystring.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_expected, basic) {
  {
    auto e = unexpected(123);
    EXPECT_EQ(e.error(), 123);
  }
  {
    expected<int, double> a;
    EXPECT_TRUE(a);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), int{});
    a.value() = 5;
    EXPECT_EQ(a.value(), 5);
  }
  {
    expected<int, double> a = 5;
    EXPECT_EQ(a.value(), 5);
    expected<int, double> b = 'a';
    EXPECT_EQ(b.value(), 'a');
    EXPECT_EQ(a.value_or(444), 5);
    EXPECT_DOUBLE_EQ(a.error_or(444.4), 444.4);
  }
  {
    auto e = unexpected(123);
    expected<double, int> a(e);
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.error(), 123);
    EXPECT_EQ(a.value_or(444), 444);
    EXPECT_EQ(a.error_or(444), 123);
  }
}

TEST(test_expected, and_then) {
  auto f = [](int x) { return expected<string, double>(to_string(x)); };
  expected<int, double> a(123), b(unexpect, 4.5);
  auto ra = a.and_then(f);
  static_assert(same_as<remove_cvref_t<decltype(ra)>, expected<string, double>>);
  EXPECT_EQ(ra.value(), "123");
  auto rb = b.and_then(f);
  EXPECT_DOUBLE_EQ(rb.error(), 4.5);
}

TEST(test_expected, transform) {
  auto f = [](int x) { return to_string(x); };
  expected<int, double> a(123), b(unexpect, 4.5);
  auto ra = a.transform(f);
  static_assert(same_as<remove_cvref_t<decltype(ra)>, expected<string, double>>);
  EXPECT_EQ(ra.value(), "123");
  auto rb = b.transform(f);
  EXPECT_DOUBLE_EQ(rb.error(), 4.5);
}

TEST(test_expected, transform_error) {
  auto f = [](double x) { return to_string(int(x * 2)); };
  expected<int, double> a(123), b(unexpect, 4.5);
  auto ra = a.transform_error(f);
  EXPECT_EQ(ra.value(), 123);
  auto rb = b.transform_error(f);
  static_assert(same_as<remove_cvref_t<decltype(rb)>, expected<int, string>>);
  EXPECT_EQ(rb.error(), "9");
}

TEST(test_expected, swap) {
  {
    expected<int, string> a(in_place, 55), b(unexpect, "3.3");
    EXPECT_TRUE(a);
    EXPECT_FALSE(b);
    swap(a, b);
    EXPECT_FALSE(a);
    EXPECT_TRUE(b);
    EXPECT_EQ(a.error(), "3.3");
    EXPECT_EQ(b.value(), 55);
  }
  {
    expected<int, string> a(unexpect, "3.3"), b(in_place, 55);
    swap(a, b);
    EXPECT_FALSE(b);
    EXPECT_TRUE(a);
    EXPECT_EQ(b.error(), "3.3");
    EXPECT_EQ(a.value(), 55);
  }
}
