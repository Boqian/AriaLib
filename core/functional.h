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

//-----------------------reference_wrapper, cref, cref-----------------------

template <class T> class reference_wrapper {
public:
  using type = T;

  template <class U>
    requires !is_same_v<remove_cvref_t<U>, reference_wrapper>
             constexpr reference_wrapper(U && x) noexcept {
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

//-----------------------aria::function-----------------------

template <typename> class function {};

template <class Ret, class... Args> class function<Ret(Args...)> {
public:
  template <class T> function(T &&t) : ptr(make_unique<Callable<T>>(forward<T>(t))) {}

  function() = default;
  function(const function &) = delete;
  function(function &&rhs) noexcept : ptr(move(rhs.ptr)) {}
  function &operator=(const function &) = delete;
  function &operator=(function &&rhs) noexcept {
    ptr = move(rhs.ptr);
    return *this;
  }

  Ret operator()(Args... args) const { return ptr->invoke(forward<Args>(args)...); }

  operator bool() const noexcept { return ptr; }

  void swap(function &rhs) noexcept { aria::swap(ptr, rhs.ptr); }

private:
  struct ICallable {
  public:
    ICallable() = default;
    virtual ~ICallable() = default;
    virtual Ret invoke(Args...) const = 0;
  };

  template <class T> struct Callable : public ICallable {
    Callable(T &&at) : t(forward<T>(at)) {}
    ~Callable() = default;

    Ret invoke(Args... args) const override { return t(forward<Args>(args)...); }

  private:
    T t;
  };

  unique_ptr<ICallable> ptr;
};

} // namespace aria