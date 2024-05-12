#pragma once
#include "utility.h"
#include <stdexcept>

namespace aria {

template <size_t N> class bitset {
public:
  class reference {
  public:
    constexpr reference(bitset<N> &a_bs, size_t a_pos) : bs(a_bs), pos(a_pos) {}
    constexpr operator bool() const noexcept { return bs.get(pos); }

    constexpr reference &operator=(bool value) noexcept {
      bs.set(pos, value);
      return *this;
    }

  private:
    bitset<N> &bs;
    size_t pos;
  };

  constexpr bitset() noexcept = default;
  constexpr bitset(unsigned long val) { v[0] = val; }
  constexpr bitset(unsigned long long val) { v[0] = val; }

  constexpr bitset &reset(size_t i) { return set(i, false); }

  constexpr bitset &set(size_t i, bool value = true) {
    auto [words_idx, bit_idx] = get_idx(i);
    v[words_idx] &= ~(1 << bit_idx);
    if (value) {
      v[words_idx] |= (1 << bit_idx);
    }
    return *this;
  }

  constexpr bool operator[](size_t i) const { return get(i); }
  constexpr reference operator[](size_t i) { return reference(*this, i); }

  constexpr unsigned long long to_ullong() const {
    if constexpr (N > bits_per_byte * sizeof(unsigned long long)) {
      throw std::overflow_error("overflow");
    } else {
      return v[0];
    }
  }

private:
  constexpr bool get(size_t i) const {
    auto [words_idx, bit_idx] = get_idx(i);
    return v[words_idx] & (1 << bit_idx);
  }

  constexpr pair<size_t, size_t> get_idx(size_t i) const { return {i / bits_per_word, i % bits_per_word}; }

  using T = unsigned long long;
  static constexpr ptrdiff_t bits_per_byte = 8;
  static constexpr ptrdiff_t bits_per_word = bits_per_byte * sizeof(T);
  static constexpr ptrdiff_t words = N / bits_per_word + (N % bits_per_word != 0 ? 1 : 0);
  T v[words] = {};
};
} // namespace aria