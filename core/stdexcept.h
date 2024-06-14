#pragma once

#include <exception.h>

namespace aria {

class logic_error : public exception {
public:
  using base = exception;
  explicit logic_error(const char *msg) : base(msg) {}
  explicit logic_error() : base() {}
};

class domain_error : public logic_error {
public:
  using base = logic_error;
  explicit domain_error(const char *msg) : base(msg) {}
};

class invalid_argument : public logic_error {
public:
  using base = logic_error;
  explicit invalid_argument(const char *msg) : base(msg) {}
};

class length_error : public logic_error {
public:
  using base = logic_error;
  explicit length_error(const char *msg) : base(msg) {}
};

class out_of_range : public logic_error {
public:
  using base = logic_error;
  //  explicit out_of_range(const string &msg) : base(msg.c_str()) {}
  explicit out_of_range(const char *msg) : base(msg) {}
};

class runtime_error : public exception {
public:
  using base = exception;
  explicit runtime_error(const char *msg) : base(msg) {}
};

class overflow_error : public runtime_error {
public:
  using base = runtime_error;
  explicit overflow_error(const char *msg) : base(msg) {}
};

class underflow_error : public runtime_error {
public:
  using base = runtime_error;
  explicit underflow_error(const char *msg) : base(msg) {}
};

class range_error : public runtime_error {
public:
  using base = runtime_error;
  explicit range_error(const char *msg) : base(msg) {}
};

//[[noreturn]] inline void _Throw_range_error(const char *const msg) { _THROW(range_error{msg}); }

} // namespace aria