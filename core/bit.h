#pragma once
#include "concepts.h"
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

template <unsigned_integral T> constexpr int popcount(T x) noexcept {
  constexpr int bits = 8 * sizeof(T);
  x = static_cast<T>(x - ((x >> 1) & static_cast<T>(0x5555'5555'5555'5555ull)));
  x = static_cast<T>((x & static_cast<T>(0x3333'3333'3333'3333ull)) + ((x >> 2) & static_cast<T>(0x3333'3333'3333'3333ull)));
  x = static_cast<T>((x + (x >> 4)) & static_cast<T>(0x0F0F'0F0F'0F0F'0F0Full));
  x = static_cast<T>(x * static_cast<T>(0x0101'0101'0101'0101ull));
  return static_cast<int>(x >> (bits - 8));
}

} // namespace aria