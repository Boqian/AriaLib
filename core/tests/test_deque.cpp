#include "deque.h"
#include "gtest/gtest.h"

using namespace aria;

static_assert(random_access_iterator<deque<int>::iterator>);
static_assert(random_access_iterator<deque<int>::const_iterator>);

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

TEST(test_deque, big5) {
  const deque<int> a = {1, 2, 3};
  {
    auto b = a;
    EXPECT_EQ(a, b);
  }
  {
    deque<int> b = {2, 3, 4};
    b = a;
    EXPECT_EQ(a, b);
  }
  {
    deque<int> b = {1, 2, 3};
    auto c = move(b);
    EXPECT_EQ(c, deque<int>({1, 2, 3}));
    EXPECT_TRUE(b.empty());
  }
  {
    deque<int> b = {1, 2, 3};
    deque<int> c = {2, 3, 4};
    c = move(b);
    EXPECT_EQ(c, deque<int>({1, 2, 3}));
    EXPECT_TRUE(b.empty());
  }
}

TEST(test_deque, desctuct) {
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

TEST(test_deque, iterator) {

  deque<int> deq;
  const int n = 4567;
  for (int i = 0; i < n; i++)
    deq.push_back(i);
  {
    int i = 0;
    for (auto it = deq.begin(); it < deq.end(); ++it, ++i) {
      EXPECT_EQ(*it, i);
    }
    for (auto it = deq.rbegin(); it < deq.rend(); --it, --i) {
      EXPECT_EQ(*it, i);
    }
  }
  {
    int i = 0, step = 313;
    for (auto it = deq.begin(); it < deq.end(); it += step, i += step) {
      EXPECT_EQ(*it, i);
    }
  }
  {
    int i = deq.back(), step = 123;
    for (auto it = deq.end() - 1; it >= deq.begin(); it -= step, i -= step) {
      EXPECT_EQ(*it, i);
    }
  }
  {
    deque<int> deq = {1, 2, 3, 4, 5};
    EXPECT_EQ(deq.end() - deq.begin(), 5);
    EXPECT_EQ(distance(deq.end(), deq.begin()), -5);
  }
}

TEST(test_deque, const_iterator) {
  const deque<int> deq = {1, 2, 3, 4, 5};
  int i = 1;
  for (auto it = deq.begin(); it < deq.end(); ++it, ++i) {
    EXPECT_EQ(*it, i);
  }
  for (auto it = deq.rbegin(); it < deq.rend(); --it, --i) {
    EXPECT_EQ(*it, i);
  }
}