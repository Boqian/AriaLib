#include "shared_ptr.h"
#include "gtest/gtest.h"
#include <utility>
using namespace aria;

struct Counter {
  static inline int n1 = 0;
  static inline int n2 = 0;
  static void init() {
    n1 = 0;
    n2 = 0;
  }
};

struct Base {
  Base() = default;
  ~Base() { Counter::n1++; } //intend not virtual
};

struct Derived : Base {
  Derived() = default;
  ~Derived() { Counter::n2++; }
};


TEST(test_shared_ptr, empty) {
  {
    auto p = shared_ptr<int>();
    EXPECT_FALSE(p);
    EXPECT_EQ(p.use_count(), 0);
  }
}

TEST(test_shared_ptr, copy_construct) {
  auto p = shared_ptr<int>(new int(100));
  EXPECT_TRUE(p);
  EXPECT_EQ(p.use_count(), 1);
  EXPECT_EQ(*p, 100);
  {
    auto q = p;
    EXPECT_TRUE(q);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(q.use_count(), 2);
    EXPECT_EQ(*q, 100);
  }
  EXPECT_EQ(p.use_count(), 1);
}

TEST(test_shared_ptr, swap) {
  auto p = shared_ptr<int>(new int(100));
  auto q = shared_ptr<int>(new int(33));
  p.swap(q);
  EXPECT_EQ(*p, 33);
  EXPECT_EQ(*q, 100);
}

TEST(test_shared_ptr, reset) {
  auto p = shared_ptr<int>(new int(100));
  p.reset();
  EXPECT_FALSE(p);
  EXPECT_EQ(p.use_count(), 0);
}

TEST(test_shared_ptr, make_shard) {
  {
    auto p = make_shared<std::pair<int, double>>(1, 1.3);
    EXPECT_EQ(p->first, 1);
    EXPECT_DOUBLE_EQ(p->second, 1.3);
  }
  {
    auto p = make_shared<int>();
    EXPECT_EQ(*p, 0);
  }
}

TEST(test_shared_ptr, move_construct) {
    {
    auto q = make_shared<int>(100);
    shared_ptr<int> p(std::move(q));
    EXPECT_FALSE(q);
    EXPECT_EQ(*p, 100);
    }
    { auto q1 = make_shared<int>(100);
      auto q2 = q1;
      auto p = std::move(q1);
      EXPECT_FALSE(q1);
      EXPECT_EQ(*p, 100);
      EXPECT_EQ(*q2, 100);
      EXPECT_EQ(p.use_count(), 2);
      EXPECT_EQ(q2.use_count(), 2);
      q2.reset();
      EXPECT_FALSE(q2);
      EXPECT_EQ(p.use_count(), 1);
    }
}

TEST(test_shared_ptr, assign) {
  {
    auto q = make_shared<int>(100);
    shared_ptr<int> p;
    p = q;
    EXPECT_EQ(*p, 100);
    EXPECT_EQ(*q, 100);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(q.use_count(), 2);
  }
  {
    auto q = make_shared<int>(100);
    shared_ptr<int> p;
    p = std::move(q);
    EXPECT_EQ(*p, 100);
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_FALSE(q);
  }
}

TEST(test_shared_ptr, destructor) {
  {
    Counter::init();
    auto p = make_shared<Base>();
    EXPECT_EQ(Counter::n1, 0);
  }
  EXPECT_EQ(Counter::n1, 1);
  {
    Counter::init();
    auto p = make_shared<Base>();
    EXPECT_EQ(Counter::n1, 0);
    p.reset();
    EXPECT_EQ(Counter::n1, 1);
  }
  EXPECT_EQ(Counter::n1, 1);
  {
    Counter::init();
    auto p = make_shared<Base>();
    auto q = p;
    auto r = q;
    EXPECT_EQ(Counter::n1, 0);
    r.reset();
    EXPECT_EQ(Counter::n1, 0);
    p.reset();
    EXPECT_EQ(Counter::n1, 0);
    q.reset();
    EXPECT_EQ(Counter::n1, 1);
  }
}

TEST(test_shared_ptr, inheritance) {  
  {
    Counter::init();
    shared_ptr<Base> p(new Derived());
    EXPECT_EQ(Counter::n1, 0);
    EXPECT_EQ(Counter::n2, 0);
    p.reset();
    EXPECT_EQ(Counter::n1, 1);
    EXPECT_EQ(Counter::n2, 1);
  }
}

TEST(test_shared_ptr, type_erase) {
  {
    Counter::init();
    shared_ptr<void> p(new Derived());
    EXPECT_EQ(Counter::n1, 0);
    EXPECT_EQ(Counter::n2, 0);
    p.reset();
    EXPECT_EQ(Counter::n1, 1);
    EXPECT_EQ(Counter::n2, 1);
  }
  {
    Counter::init();
    shared_ptr<Base> p(new Derived());
    EXPECT_EQ(Counter::n1, 0);
    EXPECT_EQ(Counter::n2, 0);
    p.reset();
    EXPECT_EQ(Counter::n1, 1);
    EXPECT_EQ(Counter::n2, 1);
  }
}