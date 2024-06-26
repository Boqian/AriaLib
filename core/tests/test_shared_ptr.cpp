#include "shared_ptr.h"
#include "gtest/gtest.h"

using namespace aria;

struct Counter {
  static inline int n_bdtor = 0;
  static inline int n_ddtor = 0;
  static inline int n_bctor = 0;
  static inline int n_dctor = 0;
  static inline int n_Actor = 0;
  static inline int n_Adtor = 0;
  static void init() {
    n_bctor = 0;
    n_bdtor = 0;
    n_dctor = 0;
    n_ddtor = 0;
    n_Actor = 0;
    n_Adtor = 0;
  }
};

struct Base {
  Base() { Counter::n_bctor++; }
  ~Base() { Counter::n_bdtor++; } // intend not virtual
};

struct Derived : Base {
  Derived() { Counter::n_dctor++; }
  ~Derived() { Counter::n_ddtor++; }
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
    auto p = make_shared<pair<int, double>>(1, 1.3);
    EXPECT_EQ(p->first, 1);
    EXPECT_DOUBLE_EQ(p->second, 1.3);
    p->first = 333;
    EXPECT_EQ(p->first, 333);
  }
  {
    auto p = make_shared<int>();
    EXPECT_EQ(*p, 0);
  }
}

TEST(test_shared_ptr, move_construct) {
  {
    auto q = make_shared<int>(100);
    shared_ptr<int> p(move(q));
    EXPECT_FALSE(q);
    EXPECT_EQ(*p, 100);
  }
  {
    auto q1 = make_shared<int>(100);
    auto q2 = q1;
    auto p = move(q1);
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
    p = move(q);
    EXPECT_EQ(*p, 100);
    EXPECT_EQ(p.use_count(), 1);
    EXPECT_FALSE(q);
  }
}

TEST(test_shared_ptr, destructor) {
  {
    Counter::init();
    auto p = make_shared<Base>();
    EXPECT_EQ(Counter::n_bdtor, 0);
  }
  EXPECT_EQ(Counter::n_bdtor, 1);
  {
    Counter::init();
    auto p = make_shared<Base>();
    EXPECT_EQ(Counter::n_bdtor, 0);
    p.reset();
    EXPECT_EQ(Counter::n_bdtor, 1);
  }
  EXPECT_EQ(Counter::n_bdtor, 1);
  {
    Counter::init();
    auto p = make_shared<Base>();
    auto q = p;
    auto r = q;
    EXPECT_EQ(Counter::n_bdtor, 0);
    r.reset();
    EXPECT_EQ(Counter::n_bdtor, 0);
    p.reset();
    EXPECT_EQ(Counter::n_bdtor, 0);
    q.reset();
    EXPECT_EQ(Counter::n_bdtor, 1);
  }
}

TEST(test_shared_ptr, inheritance) {
  {
    Counter::init();
    shared_ptr<Base> p(new Derived());
    EXPECT_EQ(Counter::n_bdtor, 0);
    EXPECT_EQ(Counter::n_ddtor, 0);
    p.reset();
    EXPECT_EQ(Counter::n_bdtor, 1);
    EXPECT_EQ(Counter::n_ddtor, 1);
  }
}

TEST(test_shared_ptr, type_erase) {
  {
    Counter::init();
    shared_ptr<void> p(new Derived());
    EXPECT_EQ(Counter::n_bdtor, 0);
    EXPECT_EQ(Counter::n_ddtor, 0);
    p.reset();
    EXPECT_EQ(Counter::n_bdtor, 1);
    EXPECT_EQ(Counter::n_ddtor, 1);
  }
  {
    Counter::init();
    shared_ptr<Base> p(new Derived());
    EXPECT_EQ(Counter::n_bdtor, 0);
    EXPECT_EQ(Counter::n_ddtor, 0);
    p.reset();
    EXPECT_EQ(Counter::n_bdtor, 1);
    EXPECT_EQ(Counter::n_ddtor, 1);
  }
}

TEST(test_weak_ptr, basic) {
  {
    Counter::init();
    auto s = make_shared<Base>();
    weak_ptr<Base> w(s);
    EXPECT_FALSE(w.expired());
    EXPECT_EQ(w.use_count(), 1);
    EXPECT_EQ(Counter::n_bctor, 1);
    EXPECT_EQ(Counter::n_bdtor, 0);
    s.reset();
    EXPECT_EQ(Counter::n_bctor, 1);
    EXPECT_EQ(Counter::n_bdtor, 1);
    EXPECT_TRUE(w.expired());
    EXPECT_EQ(w.use_count(), 0);
    auto locked = w.lock();
    EXPECT_FALSE(locked);
    w.reset();
    EXPECT_EQ(Counter::n_bctor, 1);
    EXPECT_EQ(Counter::n_bdtor, 1);
  }
  {
    Counter::init();
    auto s = make_shared<Base>();
    weak_ptr<Base> w(s);
    auto s2 = w.lock();
    EXPECT_EQ(w.use_count(), 2);
    s.reset();
    EXPECT_EQ(w.use_count(), 1);
    EXPECT_EQ(Counter::n_bctor, 1);
    EXPECT_EQ(Counter::n_bdtor, 0);
    EXPECT_FALSE(w.expired());
    w.reset();
    EXPECT_EQ(Counter::n_bdtor, 0);
    s2.reset();
    EXPECT_EQ(Counter::n_bctor, 1);
    EXPECT_EQ(Counter::n_bdtor, 1);
  }
}

struct A : enable_shared_from_this<A> {
  A() { Counter::n_Actor++; }
  ~A() { Counter::n_Adtor++; }
};

TEST(test_shared_ptr, shared_from_this) {
  Counter::init();
  auto p = make_shared<A>();
  EXPECT_EQ(p.use_count(), 1);
  auto q = p->shared_from_this();
  EXPECT_EQ(p.use_count(), 2);
  EXPECT_EQ(q.use_count(), 2);
  EXPECT_EQ(Counter::n_Actor, 1);
  EXPECT_EQ(Counter::n_Adtor, 0);
  p.reset();
  EXPECT_EQ(Counter::n_Actor, 1);
  EXPECT_EQ(Counter::n_Adtor, 0);
  EXPECT_EQ(q.use_count(), 1);
  q.reset();
  EXPECT_EQ(Counter::n_Actor, 1);
  EXPECT_EQ(Counter::n_Adtor, 1);
}

struct B1 {
  B1(int a) : x(a) {}
  int x;
  virtual ~B1() = default;
};
struct D1 : B1 {
  using B1::B1;
};

TEST(test_shared_ptr, pointer_cast) {
  {
    auto p = make_shared<int>(5);
    auto q = const_pointer_cast<const int>(p);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(q.use_count(), 2);
    EXPECT_EQ(*q, 5);
    *p = 111;
    EXPECT_EQ(*q, 111);
  }
  {
    shared_ptr<B1> p(new D1(5));
    auto q = dynamic_pointer_cast<D1>(p);
    EXPECT_EQ(p.use_count(), 2);
    EXPECT_EQ(q.use_count(), 2);
    EXPECT_EQ(q->x, 5);
    q->x = 111;
    EXPECT_EQ(p->x, 111);
  }
}

TEST(test_shared_ptr, construct_with_deleter) {
  int delete_result = 0;
  auto deletor = [&](int *p) {
    delete p;
    delete_result += 666;
  };
  { shared_ptr<int> sp(new int(5), deletor); }
  EXPECT_EQ(delete_result, 666);
}

TEST(test_shared_ptr, construct_from_weak) {
  auto p1 = make_shared<int>(5);
  weak_ptr<int> w(p1);
  shared_ptr<int> p2(w);
  EXPECT_EQ(*p2, 5);
}

TEST(test_shared_ptr, construct_from_unique) {
  auto p1 = make_unique<int>(5);
  shared_ptr<int> p2(move(p1));
  EXPECT_EQ(*p2, 5);
  EXPECT_FALSE(p1);
}

TEST(test_shared_ptr, make_shared_for_array) {
  {
    auto p = make_shared<int[3]>();
    EXPECT_EQ(p[0], 0);
    EXPECT_EQ(p[1], 0);
    EXPECT_EQ(p[2], 0);
    p[2] = 666;
    EXPECT_EQ(p[2], 666);
  }
  {
    auto p = make_shared<int[]>(3);
    EXPECT_EQ(p[0], 0);
    EXPECT_EQ(p[1], 0);
    EXPECT_EQ(p[2], 0);
    p[2] = 666;
    EXPECT_EQ(p[2], 666);
  }
  {
    auto p = make_shared_for_overwrite<int>();
    auto q = make_shared_for_overwrite<int[]>(5);
  }
}
