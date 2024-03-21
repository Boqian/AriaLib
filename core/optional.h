#pragma once

namespace aria {

template <class T> class optional {
public:
  using value_type = T;
  constexpr optional() noexcept : empty{} {}

  constexpr operator bool() const noexcept { return has_value; }

private:
  struct empty_byte {};
  union {
    empty_byte empty;
    T value;
  };
  bool has_value = false;
};

}