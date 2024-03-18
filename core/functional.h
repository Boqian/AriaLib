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

  Ret operator()(Args... args) {
    return ptr->invoke(std::forward<Args>(args)...);
  }

private:
  struct ICallable {
   public:
    ICallable() = default;
    virtual ~ICallable() = default;
    virtual Ret invoke(Args...) = 0;
  };

  template <class T>
  struct Callable : public ICallable {
    Callable(T&& at) : t(std::forward<T>(at)) {}
    ~Callable() = default;

    Ret invoke(Args... args) override { return t(std::forward<Args>(args)...); }

   private:
    T t;
  };

  unique_ptr<ICallable> ptr;
};

}  // namespace aria