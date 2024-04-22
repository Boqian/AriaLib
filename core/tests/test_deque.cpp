#include "deque.h"
#include "gtest/gtest.h"

using namespace aria;

struct A {
  static inline int n_ctor = 0;
  static inline int n_dtor = 0;
  static void reset() {
    n_ctor = 0;
    n_dtor = 0;
  }
  A() { n_ctor++; }
  A(const A &) { n_ctor++; }
  A(A &&) = default;
  ~A() { n_dtor++; }
};

TEST(test_deque, basic) {
  {
    deque<int> deq;
    EXPECT_TRUE(deq.empty());
    EXPECT_EQ(deq.size(), 0);
    deq.push_back(44);
    EXPECT_FALSE(deq.empty());
    EXPECT_EQ(deq.size(), 1);
    EXPECT_EQ(deq[0], 44);
    deq[0] = 55;
    EXPECT_EQ(deq[0], 55);
  }
  {
    deque<int> deq;
    const int n = 1000;
    for (int i = 0; i < n; i++) {
      deq.push_back(i * 10);
      EXPECT_EQ(deq.size(), i + 1);
      EXPECT_EQ(deq[i], i * 10);
    }
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(deq[i], i * 10);
    }
    for (int i = n - 1; i >= 0; i--) {
      EXPECT_EQ(deq.back(), i * 10);
      deq.pop_back();
      EXPECT_EQ(deq.size(), i);
    }
  }
  {
    deque<int> deq;
    const int n = 1000;
    for (int i = 0; i < n; i++) {
      deq.push_front(i * 10);
      EXPECT_EQ(deq.size(), i + 1);
      EXPECT_EQ(deq[0], i * 10);
    }
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(deq[i], (n - i - 1) * 10);
    }
    for (int i = n - 1; i >= 0; i--) {
      EXPECT_EQ(deq.front(), i * 10);
      deq.pop_front();
      EXPECT_EQ(deq.size(), i);
    }
  }
  {
    deque<int> deq = {1, 2, 3, 4, 5};
    EXPECT_EQ(deq.size(), 5);
    EXPECT_EQ(deq.front(), 1);
    EXPECT_EQ(deq.back(), 5);
  }
}

TEST(test_deque, construct) {
  A a;
  A::reset();
  const int n = 1000;
  {
    deque<A> deq;
    for (int i = 0; i < n; i++)
      deq.push_back(a);
    EXPECT_EQ(A::n_ctor, n);
    EXPECT_EQ(A::n_dtor, 0);
  }
  EXPECT_EQ(A::n_ctor, n);
  EXPECT_EQ(A::n_dtor, n);
  A::reset();
  {
    deque<A> deq;
    for (int i = 0; i < n; i++)
      deq.push_back(a);
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(A::n_ctor, n);
      EXPECT_EQ(A::n_dtor, i);
      deq.pop_back();
    }
    EXPECT_EQ(A::n_dtor, n);
  }
  A::reset();
  {
    deque<A> deq;
    for (int i = 0; i < n; i++)
      deq.push_back(a);
    for (int i = 0; i < n; i++) {
      EXPECT_EQ(A::n_ctor, n);
      EXPECT_EQ(A::n_dtor, i);
      deq.pop_front();
    }
    EXPECT_EQ(A::n_dtor, n);
  }
}