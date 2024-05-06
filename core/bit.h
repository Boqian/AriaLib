#pragma once
#include "utility.h"

// https://en.cppreference.com/w/cpp/header/bit

namespace aria {

enum class endian {
  little = 0,
  big = 1,
#if defined(_MSC_VER) && !defined(__clang__)
  native = little
#else
  native = big
#endif
};

template <class To, class From>
  requires(sizeof(To) == sizeof(From))
constexpr To bit_cast(const From &from) {
  To to;
  memcpy(&to, &from, sizeof(to));
  return to;
}

} // namespace aria