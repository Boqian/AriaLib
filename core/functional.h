#pragma once
#include <utility>
#include "unique_ptr.h"

namespace aria {

template <typename>
class function {};

template <class Ret, class... Args>
class function<Ret(Args...)> {
 public:
  template<class T>
  function(T&& t)
      : ptr(make_unique<Callable<T>>(std::forward<T>(t))) {}

  function() = default;
  function(const function&) = delete;
  function& operator=(const function&) = delete;
  function& operator=(function&& rhs) noexcept { return ptr = std::move(rhs.ptr); }
  function(function&& rhs) noexcept : ptr(std::move(rhs.ptr)) {}

  Ret operator()(Args... args) const {
    return ptr->invoke(std::forward<Args>(args)...);
  }

  operator bool() const noexcept { return ptr; }

  void swap(function& rhs) noexcept { std::swap(ptr, rhs.ptr); }

private:
  struct ICallable {
   public:
    ICallable() = default;
    virtual ~ICallable() = default;
    virtual Ret invoke(Args...) const = 0;
  };

  template <class T>
  struct Callable : public ICallable {
    Callable(T&& at) : t(std::forward<T>(at)) {}
    ~Callable() = default;

    Ret invoke(Args... args) const override { return t(std::forward<Args>(args)...); }

   private:
    T t;
  };

  unique_ptr<ICallable> ptr;
};

}  // namespace aria