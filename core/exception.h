#pragma once

namespace aria {

class exception {
public:
  exception() noexcept = default;
  virtual ~exception() noexcept = default;
  explicit exception(char const *const message) noexcept : msg(message) {}
  exception(exception const &rhs) noexcept : msg(rhs.msg) {}
  exception &operator=(exception const &rhs) noexcept {
    msg = rhs.msg;
    return *this;
  }

  [[nodiscard]] virtual char const *what() const { return msg ? msg : "Unknown exception"; }

private:
  char const *msg = nullptr;
};

class bad_exception : public exception {
public:
  bad_exception() noexcept : exception("bad exception") {}
};

class bad_alloc : public exception {
public:
  bad_alloc() noexcept : exception("bad allocation") {}
  using exception::exception;
};

class bad_array_new_length : public bad_alloc {
public:
  bad_array_new_length() noexcept : bad_alloc("bad array new length") {}
};

} // namespace aria