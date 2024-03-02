#include <memory>
#include <shared_mutex>

namespace aria {

/*
a simple implementation of folly::Synchronized
https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md
*/

template <class T, class LockType>
class LockedPtr {
 public:
  using mutex_type = LockType::mutex_type;
  LockedPtr(T* a_ptr, mutex_type& mut, bool try_to_lock = false)
      : ptr(a_ptr),
        lock(try_to_lock ? LockType(mut, std::try_to_lock)
                         : LockType(mut)) {}
  ~LockedPtr() = default;

  T* operator->() noexcept { return ptr; }
  const T* operator->() const noexcept { return ptr; }

  T& operator*() { return *ptr; }
  const T& operator*() const { return *ptr; }

  bool is_locked() const { return lock.owns_lock(); }
  void unlock() { lock.unlock(); }

  LockType& as_lock() noexcept { return lock; }
  LockType const& as_lock() const noexcept { return lock; }

 private:
  T* ptr;
  LockType lock;
};

template <class T, class TMutex> 
class SynchronizedBase {
};

template <class T>
class SynchronizedBase<T, std::shared_mutex> {
 public:
  using mutex_type = std::shared_mutex;
  using wlock_type = std::unique_lock<mutex_type>;
  using rlock_type = std::shared_lock<mutex_type>;
  using wlocked_ptr_type = LockedPtr<T, wlock_type>;
  using rlocked_ptr_type = LockedPtr<const T, rlock_type>;

  SynchronizedBase() : obj{} {}

  auto wlock() { return wlocked_ptr_type(&obj, mut); }
  auto rlock() { return rlocked_ptr_type(&obj, mut); }
  auto try_wlock() { return wlocked_ptr_type(&obj, mut, true); }
  auto try_rlock() { return rlocked_ptr_type(&obj, mut, false); }

 private:
  mutex_type mut;
  T obj;
};

template <class T>
class SynchronizedBase<T, std::mutex> {
 public:
  using mutex_type = std::mutex;
  using lock_type = std::unique_lock<mutex_type>;
  using locked_ptr_type = LockedPtr<T, lock_type>;
  SynchronizedBase() : obj{} {}

  auto lock() { return locked_ptr_type(&obj, mut); }
  auto try_lock() { return locked_ptr_type(&obj, mut, true); }

private:
  mutex_type mut;
  T obj;
};


template <class T, class MutexType = std::shared_mutex>
class Synchronized : public SynchronizedBase<T, MutexType> {
 public:

};

}  // namespace aria