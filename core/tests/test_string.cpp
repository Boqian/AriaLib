#include "mystring.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_string, empty_string) {
  {
    string a;
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(0, a.size());
  }
  { string a = ""; }
}

TEST(test_string, basic) {
  {
    string a = "123";
    EXPECT_FALSE(a.empty());
    EXPECT_EQ(a.size(), 3);
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
  {
    string a = "123";
    a.clear();
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a, "");
  }
}

TEST(test_string, operation) {
  {
    string a = "12345";
    EXPECT_EQ(a.substr(0, 2), "12");
    EXPECT_EQ(a.substr(1, 1), "2");
    EXPECT_EQ(a.substr(3), "45");
  }
  {
    string a;
    EXPECT_EQ((a += '1'), "1");
    EXPECT_EQ((a += '2'), "12");
    EXPECT_EQ((a += '3'), "123");
  }
  {
    string a = "1", b = "987";
    EXPECT_EQ((a += b), "1987");
    EXPECT_EQ((a += b), "1987987");
    EXPECT_EQ((a += b), "1987987987");
  }
  {
    string a = "1";
    const char *b = "987";
    EXPECT_EQ((a += b), "1987");
    EXPECT_EQ((a += b), "1987987");
    EXPECT_EQ((a += b), "1987987987");
  }
  {
    string a = "";
    a.push_back('1');
    EXPECT_EQ(a, "1");
    a.pop_back();
    EXPECT_EQ(a, "");
  }
}

TEST(test_string, iterator) {
  {
    string a = "12345";
    auto beg = a.begin(), ed = a.end();
    EXPECT_EQ(*beg, '1');
    EXPECT_EQ(*(beg + 1), '2');
    EXPECT_EQ(*(ed - 2), '4');
    int sum = 0;
    for (auto c : a)
      sum += c - '0';
    EXPECT_EQ(sum, 15);
  }
  {
    const string a = "12345";
    string b;
    for (auto it = a.rbegin(); it != a.rend(); ++it)
      b += *it;
    EXPECT_EQ(b, "54321");
  }
}

TEST(test_string, c_str) {
  string a = "12345";
  EXPECT_EQ(strlen(a.c_str()), 5);
  *(a.data() + 2) = 'a';
  EXPECT_EQ(a, "12a45");
}

TEST(test_string, swap) {
  string a = "123", b = "456";
  a.swap(b);
  EXPECT_EQ(a, "456");
  EXPECT_EQ(b, "123");
}

TEST(test_string, compare) {
  string a = "123", b = "456", c = "13";
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a < c);
  EXPECT_TRUE(c < b);
}

TEST(test_string, to_int) {
  string a = "123", b = "3.5";
  EXPECT_EQ(stoi(a), 123);
  EXPECT_DOUBLE_EQ(stof(b), 3.5);
  EXPECT_DOUBLE_EQ(stod(b), 3.5);
}