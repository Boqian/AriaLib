#include "mystring.h"
#include "string_view.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_string_view, basic) {
  {
    string_view sv;
    EXPECT_TRUE(sv.empty());
    EXPECT_EQ(sv.size(), 0);
  }
  {
    const char *s = "12345";
    string_view sv(s);
    EXPECT_EQ(sv.size(), 5);
    EXPECT_EQ(sv[0], '1');
  }
  {
    string s = "12345";
    string_view sv(s);
    EXPECT_EQ(sv.size(), 5);
    EXPECT_EQ(sv[0], '1');
  }
  {
    string s = "12345";
    string_view sv(s.begin() + 1, s.begin() + 2);
    EXPECT_EQ(sv.size(), 1);
    EXPECT_EQ(sv[0], '2');
  }
}

TEST(test_string_view, compare) {
  {
    string s = "12345", t = "123";
    string_view a(s.data(), 3), b(s.data(), 4), c(s), d(t), e(s.data() + 2, 3);
    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(b, c);
    EXPECT_EQ(a, d);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a.compare(b) == -1);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(b.compare(a) == 1);
    EXPECT_TRUE(d.compare(a) == 0);
    EXPECT_TRUE(b.starts_with(a));
    EXPECT_TRUE(c.starts_with(a));
    EXPECT_TRUE(c.starts_with(b));
    EXPECT_TRUE(d.starts_with(a));
    EXPECT_FALSE(a.starts_with(b));
    EXPECT_TRUE(c.ends_with(e));
    EXPECT_TRUE(a.ends_with(d));
    EXPECT_FALSE(e.ends_with(c));
    EXPECT_FALSE(e.ends_with(a));
  }
}