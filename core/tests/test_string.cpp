#include "mystring.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_string, empty_string) {
  string a;
  EXPECT_TRUE(a.empty());
  EXPECT_EQ(0, a.size());
  EXPECT_EQ(0, a.capacity());
}

TEST(test_string, basic) {
  {
    string a = "123";
    EXPECT_FALSE(a.empty());
    EXPECT_EQ(a.size(), 3);
    EXPECT_EQ(a.capacity(), 3);
    EXPECT_EQ(a[0], '1');
    EXPECT_EQ(a[1], '2');
    EXPECT_EQ(a.back(), '3');
    EXPECT_EQ(a, "123");
    a[0] = 's';
    EXPECT_EQ(a, "s23");
  }
  {
    string a = "123";
    string b = a;
    EXPECT_EQ(b, "123");
  }
  {
    string a = "123";
    string b = move(a);
    EXPECT_EQ(b, "123");
    EXPECT_EQ(a, "");
    EXPECT_TRUE(a.empty());
  }
  {
    string a = "123", b = "44";
    b = a;
    EXPECT_EQ(b, "123");
    EXPECT_EQ(a, "123");
  }
  {
    string a = "123", b = "44";
    b = move(a);
    EXPECT_EQ(b, "123");
    EXPECT_EQ(a, "");
  }
}