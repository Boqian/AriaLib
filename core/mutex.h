#pragma once

namespace aria {

class mutex {
public:
  constexpr mutex() noexcept = default;
  mutex(const mutex &) = delete;
  mutex &operator=(const mutex &) = delete;

  void lock();
  void unlock();
  bool try_lock();

private:
  bool locked{};
};

} // namespace aria