#pragma once
#include "array.h"
#include "concepts.h"

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

template <integral T> constexpr T byteswap(T x) noexcept {
  auto bits = bit_cast<array<char, sizeof(T)>>(x);
  reverse(begin(bits), end(bits));
  return bit_cast<T>(bits);
}

template <unsigned_integral T> constexpr bool has_single_bit(T x) noexcept { return x && !(x & (x - 1)); }

template <unsigned_integral T> constexpr int countl_zero(T x) noexcept {
  T y = 0;
  unsigned int bits = 8 * sizeof(T), cx = bits / 2;
  do {
    y = static_cast<T>(x >> cx);
    if (y != 0) {
      bits -= cx;
      x = y;
    }
    cx >>= 1;
  } while (cx != 0);
  return static_cast<int>(bits) - static_cast<int>(x);
}

template <unsigned_integral T> constexpr int bit_width(T x) noexcept { return 8 * sizeof(T) - countl_zero(x); }

template <unsigned_integral T> constexpr int popcount(T x) noexcept {
  constexpr int bits = 8 * sizeof(T);
  x = static_cast<T>(x - ((x >> 1) & static_cast<T>(0x5555'5555'5555'5555ull)));
  x = static_cast<T>((x & static_cast<T>(0x3333'3333'3333'3333ull)) + ((x >> 2) & static_cast<T>(0x3333'3333'3333'3333ull)));
  x = static_cast<T>((x + (x >> 4)) & static_cast<T>(0x0F0F'0F0F'0F0F'0F0Full));
  x = static_cast<T>(x * static_cast<T>(0x0101'0101'0101'0101ull));
  return static_cast<int>(x >> (bits - 8));
}

template <unsigned_integral T> constexpr T bit_ceil(T x) noexcept {
  if (x != 0)
    return T(1) << bit_width(x - 1);
  return 1;
}

template <unsigned_integral T> constexpr T bit_floor(T x) noexcept {
  if (x != 0)
    return T{1} << (bit_width(x) - 1);
  return 0;
}

} // namespace aria