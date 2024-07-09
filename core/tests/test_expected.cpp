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