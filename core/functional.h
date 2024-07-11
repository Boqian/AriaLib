#pragma once
#include "unique_ptr.h"
#include "utility.h"

namespace aria {

//-----------------------Comparisons-----------------------
template <class T = void> struct less {
  constexpr bool operator()(const T &a, const T &b) const { return a < b; }
};
template <> struct less<void> {
  template <class T> constexpr bool operator()(const T &a, const T &b) const { return a < b; }
};

template <class T = void> struct greater {
  constexpr bool operator()(const T &a, const T &b) const { return a > b; }
};
template <> struct greater<void> {
  template <class T> constexpr bool operator()(const T &a, const T &b) const { return a > b; }
};

template <class T = void> struct equal_to {
  constexpr bool operator()(const T &a, const T &b) const { return a == b; }
};
template <> struct equal_to<void> {
  template <class T> constexpr bool operator()(const T &a, const T &b) const { return a == b; }
};

template <class T = void> struct not_equal_to {
  constexpr bool operator()(const T &a, const T &b) const { return a != b; }
};
template <> struct not_equal_to<void> {
  template <class T> constexpr bool operator()(const T &a, const T &b) const { return a != b; }
};

template <class T = void> struct greater_equal {
  constexpr bool operator()(const T &a, const T &b) const { return a >= b; }
};
template <> struct greater_equal<void> {
  template <class T> constexpr bool operator()(const T &a, const T &b) const { return a >= b; }
};

template <class T = void> struct less_equal {
  constexpr bool operator()(const T &a, const T &b) const { return a <= b; }
};
template <> struct less_equal<void> {
  template <class T> constexpr bool operator()(const T &a, const T &b) const { return a <= b; }
};

//-----------------------Arithmetic operations-----------------------

template <class T = void> struct plus {
  constexpr T operator()(const T &a, const T &b) const { return a + b; }
};
template <> struct plus<void> {
  template <class T> constexpr T operator()(const T &a, const T &b) const { return a + b; }
};

template <class T = void> struct minus {
  constexpr T operator()(const T &a, const T &b) const { return a - b; }
};
template <> struct minus<void> {
  template <class T> constexpr T operator()(const T &a, const T &b) const { return a - b; }
};

template <class T = void> struct multiplies {
  constexpr T operator()(const T &a, const T &b) const { return a * b; }
};
template <> struct multiplies<void> {
  template <class T> constexpr T operator()(const T &a, const T &b) const { return a * b; }
};

template <class T = void> struct divides {
  constexpr T operator()(const T &a, const T &b) const { return a / b; }
};
template <> struct divides<void> {
  template <class T> constexpr T operator()(const T &a, const T &b) const { return a / b; }
};

template <class T = void> struct modulus {
  constexpr T operator()(const T &a, const T &b) const { return a % b; }
};
template <> struct modulus<void> {
  template <class T> constexpr T operator()(const T &a, const T &b) const { return a % b; }
};

template <class T = void> struct negate {
  constexpr T operator()(const T &a) const { return -a; }
};
template <> struct negate<void> {
  template <class T> constexpr T operator()(const T &a) const { return -a; }
};

//-----------------------reference_wrapper, cref, cref-----------------------
template <class T> class reference_wrapper;
template <class T> struct is_reference_wrapper : false_type {};
template <class T> struct is_reference_wrapper<reference_wrapper<T>> : true_type {};
template <class T> inline constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

template <class T> class reference_wrapper {
public:
  using type = T;

  template <class U> requires !is_reference_wrapper_v<U> constexpr reference_wrapper(U && x) noexcept {
    T &ref = static_cast<U &&>(x);
    ptr = &ref;
  }

  constexpr reference_wrapper(const reference_wrapper &other) noexcept = default;
  constexpr reference_wrapper &operator=(const reference_wrapper &rhs) noexcept { ptr = rhs.ptr; }

  constexpr T &get() const noexcept { return *ptr; }
  constexpr operator T &() const noexcept { return *ptr; }

private:
  T *ptr = nullptr;
};

template <class T> constexpr auto ref(T &t) noexcept { return reference_wrapper<T>(t); }
template <class T> void ref(const T &&) = delete;
template <class T> constexpr auto cref(const T &t) noexcept { return reference_wrapper<const T>(t); }
template <class T> void cref(const T &&) = delete;

//-----------------------hash-----------------------
inline constexpr size_t FNV_offset_basis = 14695981039346656037ULL;
inline constexpr size_t FNV_prime = 1099511628211ULL;

// accumulate range [p, p + size) into partial FNV-1a hash value
inline size_t Fnv1a_append_bytes(size_t val, const unsigned char *const p, const size_t size) noexcept {
  for (size_t i = 0; i < size; ++i) {
    val ^= static_cast<size_t>(p[i]);
    val *= FNV_prime;
  }
  return val;
}

template <class T> size_t Fnv1a_append_range(const size_t val, const T *const first, const T *const last) noexcept {
  static_assert(is_trivial_v<T>, "Only trivial types can be directly hashed.");
  const auto firstb = reinterpret_cast<const unsigned char *>(first);
  const auto lastb = reinterpret_cast<const unsigned char *>(last);
  return Fnv1a_append_bytes(val, firstb, static_cast<size_t>(lastb - firstb));
}

template <class T> size_t Fnv1a_append_value(const size_t val, const T &x) noexcept {
  static_assert(is_trivial_v<T>, "Only trivial types can be directly hashed.");
  return Fnv1a_append_bytes(val, &reinterpret_cast<const unsigned char &>(x), sizeof(T));
}

template <class T> size_t Hash_representation(const T &x) noexcept { return Fnv1a_append_value(FNV_offset_basis, x); }

template <class T> size_t Hash_array_representation(const T *const p, const size_t size) noexcept {
  return Fnv1a_append_bytes(FNV_offset_basis, reinterpret_cast<const unsigned char *>(p), size * sizeof(T));
}

template <class T> struct hash {};

template <class T> requires is_integral_v<T> || is_pointer_v<T> struct hash<T> {
  size_t operator()(const T x) const noexcept { return Hash_representation(x); }
};

template <class T> requires is_floating_point_v<T> struct hash<T> {
  size_t operator()(const T x) const noexcept { return Hash_representation(x == 0.0 ? 0.0 : x); }
};

template <> struct hash<nullptr_t> {
  size_t operator()(nullptr_t) const noexcept {
    void *p{};
    return Hash_representation(p);
  }
};

//-----------------------aria::invoke-----------------------
namespace functional {
template <class C, class Pointed, class Object, class... Args>
constexpr decltype(auto) invoke_memptr(Pointed C::*member, Object &&object, Args &&...args) {
  using object_t = remove_cvref_t<Object>;
  constexpr bool is_member_function = is_function_v<Pointed>;
  constexpr bool is_wrapped = is_reference_wrapper_v<object_t>;
  constexpr bool is_derived_object = is_same_v<C, object_t> || is_base_of_v<C, object_t>;

  if constexpr (is_member_function) {
    if constexpr (is_derived_object)
      return (forward<Object>(object).*member)(forward<Args>(args)...);
    else if constexpr (is_wrapped)
      return (object.get().*member)(forward<Args>(args)...);
    else
      return ((*forward<Object>(object)).*member)(forward<Args>(args)...);
  } else {
    static_assert(is_object_v<Pointed> && sizeof...(args) == 0);
    if constexpr (is_derived_object)
      return forward<Object>(object).*member;
    else if constexpr (is_wrapped)
      return object.get().*member;
    else
      return (*forward<Object>(object)).*member;
  }
}

} // namespace functional

template <class F, class... Args> constexpr auto invoke(F &&f, Args &&...args) {
  if constexpr (is_member_pointer_v<remove_cvref_t<F>>)
    return functional::invoke_memptr(f, forward<Args>(args)...);
  else
    return forward<F>(f)(forward<Args>(args)...);
}

//-----------------------aria::function-----------------------

template <typename> class function {};

template <class R, class... Args> class function<R(Args...)> {
public:
  using result_type = R;

  template <class F> function(F &&f) : ptr(make_unique<Callable<F>>(forward<F>(f))) {}

  function() noexcept = default;
  function(nullptr_t) noexcept {}
  ~function() = default;
  function(const function &) = delete;
  function(function &&rhs) noexcept : ptr(move(rhs.ptr)) {}
  function &operator=(const function &) = delete;
  function &operator=(function &&rhs) noexcept {
    ptr = move(rhs.ptr);
    return *this;
  }

  function &operator=(nullptr_t) noexcept { ptr.reset(); }

  template <class F> function &operator=(F &&f) {
    function(forward<F>(f)).swap(*this);
    return *this;
  }

  result_type operator()(Args... args) const { return ptr->invoke(forward<Args>(args)...); }

  operator bool() const noexcept { return ptr; }

  void swap(function &rhs) noexcept { aria::swap(ptr, rhs.ptr); }

private:
  struct ICallable {
  public:
    ICallable() = default;
    virtual ~ICallable() = default;
    virtual R invoke(Args...) const = 0;
  };

  template <class T> struct Callable : public ICallable {
    Callable(T &&at) : t(forward<T>(at)) {}
    ~Callable() = default;

    R invoke(Args... args) const override { return t(forward<Args>(args)...); }

  private:
    T t;
  };

  unique_ptr<ICallable> ptr;
};

template <class R, class... Args> void swap(function<R(Args...)> &a, function<R(Args...)> &b) noexcept { a.swap(b); }

} // namespace aria