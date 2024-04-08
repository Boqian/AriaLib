#pragma once
#include "unique_ptr.h"
#include "utility.h"

namespace aria {

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

} // namespace aria