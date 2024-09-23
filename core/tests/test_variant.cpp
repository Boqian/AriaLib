#include "variant.h"
#include "gtest/gtest.h"
#include "mystring.h"

using namespace aria;

static_assert(is_same_v<int, best_overload_match<char, float, int, long, double>>);
static_assert(is_same_v<double, best_overload_match<float, int, double, bool>>);

TEST(test_variant, variant_storage) {
  using var = _variant::storage<int, double, char>;
  constexpr var x(integral_constant<size_t, 0>(), 1);
  static_assert(_variant::raw_get<0>(x) == 1);
  constexpr var y(integral_constant<size_t, 2>(), 'c');
  static_assert(_variant::raw_get<2>(y) == 'c');
}

TEST(test_variant, visit_with_index) {
  using namespace _variant;
  using var = _variant::storage<int, double, char>;
  var x(integral_constant<size_t, 0>(), 1);
  var y(integral_constant<size_t, 2>(), 'c');

  auto f = [](auto &x) { x += 1; };
  _variant::visit_with_index(f, x, 0);
  EXPECT_EQ(_variant::raw_get<0>(x), 2);
  _variant::visit_with_index(f, y, 2);
  EXPECT_EQ(_variant::raw_get<2>(y), 'd');
}

TEST(test_variant, basic) {
  using var = variant<int, double, char>;
  {
    constexpr var v(123);
    static_assert(v.index() == 0);
    static_assert(holds_alternative<int>(v));
    static_assert(get<0>(v) == 123);
    static_assert(get<int>(v) == 123);
    EXPECT_EQ(v.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(v));
  }
  {
    constexpr var v(123.0);
    static_assert(v.index() == 1);
    static_assert(holds_alternative<double>(v));
    EXPECT_EQ(v.index(), 1);
    EXPECT_TRUE(holds_alternative<double>(v));
  }
  {
    constexpr var v('e');
    static_assert(v.index() == 2);
    static_assert(holds_alternative<char>(v));
    static_assert(get<char>(v) == 'e');
    EXPECT_EQ(v.index(), 2);
    EXPECT_TRUE(holds_alternative<char>(v));
  }
  {
    constexpr var v;
    static_assert(v.index() == 0);
  }
  static_assert(is_same_v<variant_alternative_t<0, var>, int>);
  static_assert(is_same_v<variant_alternative_t<1, var>, double>);
  static_assert(is_same_v<variant_alternative_t<2, var>, char>);
}

struct Counter {
  static inline int n_dtor = 0;
  static inline int n_ctor = 0;
  static inline int n_copy_ctor = 0;
  static inline int n_move_ctor = 0;
  static void init() {
    n_ctor = 0;
    n_dtor = 0;
    n_copy_ctor = 0;
    n_move_ctor = 0;
  }
};

struct A {
  explicit A() { Counter::n_ctor++;  }
  ~A() { Counter::n_dtor++; }
  A(const A &) { Counter::n_copy_ctor++; }
  A(A &&) noexcept  { Counter::n_move_ctor++; }
  
  A &operator=(const A &) = default;
  A &operator=( A &&) = default;

  int val{};
};

TEST(test_variant, destuctor) {
  using var = variant<int, A>;
  A a;
  Counter::init();
  {
    var v(a);
    EXPECT_EQ(Counter::n_copy_ctor, 1);
    EXPECT_EQ(Counter::n_dtor, 0);
  }
  EXPECT_EQ(Counter::n_dtor, 1);
}

TEST(test_variant, copy_construct) {
  using var = variant<int, A, char>;
  {
    A a;
    var v(a);
    Counter::init();
    {
      var u(v);
      EXPECT_EQ(u.index(), 1);
      EXPECT_TRUE(holds_alternative<A>(u));
      EXPECT_EQ(Counter::n_copy_ctor, 1);
      EXPECT_EQ(Counter::n_dtor, 0);
    }
    EXPECT_EQ(Counter::n_dtor, 1);
  }
  { 
    int x = 555;
    var a(x);
    var b(a);
    EXPECT_EQ(b.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(b));
    EXPECT_EQ(get<int>(b), 555);
  }
  {
    char x = 'g';
    var a(x);
    var b(a);
    EXPECT_EQ(b.index(), 2);
    EXPECT_TRUE(holds_alternative<char>(b));
    EXPECT_EQ(get<char>(b), 'g');
  }
}

TEST(test_variant, move_construct) {
  using var = variant<int, A, aria::string>;
  {
    A a;
    var v(a);
    Counter::init();
    {
      var u(move(v));
      EXPECT_EQ(u.index(), 1);
      EXPECT_TRUE(holds_alternative<A>(u));
      EXPECT_EQ(Counter::n_move_ctor, 1);
      EXPECT_EQ(Counter::n_dtor, 0);
    }
    EXPECT_EQ(Counter::n_dtor, 1);
  }
  {
    string s = "haha";
    var a(s);
    var b(move(a));
    EXPECT_EQ(b.index(), 2);
    EXPECT_TRUE(holds_alternative<string>(b));
    EXPECT_EQ(get<string>(b), "haha");
    EXPECT_EQ(a.index(), 2);
    EXPECT_TRUE(holds_alternative<string>(a));
    EXPECT_EQ(get<string>(a), "");
  }
}