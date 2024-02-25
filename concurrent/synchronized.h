#include <memory>
#include <shared_mutex>

namespace aria {

/*
a simple implementation of folly::Synchronized
https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md
*/

template <class T, class Lock>
class LockedPtr {
 public:
  LockedPtr(T* a_ptr, std::shared_mutex& mut) : ptr(a_ptr), lock{mut} {}
  ~LockedPtr() = default;
 
  T* operator->() noexcept {
    return ptr;
  }
 
  const T* operator->() const noexcept {
    return ptr;
  }

  void unlock() { lock.unlock(); }

 private:
  T* ptr;
  Lock lock;
};

template <class T>
class Synchronized {
 public:
  using Wlock = std::unique_lock<std::shared_mutex>;
  using Rlock = std::shared_lock<std::shared_mutex>;
  using WlockPtr = LockedPtr<T, Wlock>;
  using RlockPtr = LockedPtr<const T, Rlock>;

  Synchronized() : obj{} {}

  auto wlock() { return WlockPtr(&obj, mut); }
  auto rlock() { return RlockPtr(&obj, mut); }

 private:
  std::shared_mutex mut;
  T obj;
};

}  // namespace aria