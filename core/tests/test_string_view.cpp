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