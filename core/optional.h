#pragma once

namespace aria {

struct nullopt_t {
  struct Tag {};
  constexpr explicit nullopt_t(Tag) {}
};
inline constexpr auto nullopt = nullopt_t{nullopt_t::Tag{}};

template <class T> class optional {
public:
  using value_type = T;
  constexpr optional() noexcept : empty{} {}
  constexpr optional(nullopt_t) noexcept : empty{} {}
  constexpr ~optional() {
      if (has_value)
        value.~T();
  }

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