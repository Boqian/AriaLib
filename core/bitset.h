#pragma once
#include "algorithm.h"
#include "bit.h"
#include "stdexcept.h"
#include "utility.h"

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
  constexpr bitset(unsigned long long val) { v[0] = val; }

  constexpr bitset &set(size_t i, bool value = true) {
    auto [words_idx, bit_idx] = get_idx(i);
    v[words_idx] &= ~(T(1) << bit_idx);
    if (value) {
      v[words_idx] |= (T(1) << bit_idx);
    }
    return *this;
  }

  constexpr bitset &set() noexcept {
    for (auto &x : v)
      x = ~T(0);
    return *this;
  }

  constexpr bitset &reset(size_t i) { return set(i, false); }

  constexpr bitset &reset() noexcept {
    for (auto &x : v)
      x = 0;
    return *this;
  }

  constexpr bitset &flip(size_t i) {
    auto [words_idx, bit_idx] = get_idx(i);
    v[words_idx] ^= (T(1) << bit_idx);
    return *this;
  }

  constexpr bitset &flip() noexcept {
    for (auto &x : v)
      x = ~x;
    return *this;
  }

  constexpr size_t count() const noexcept {
    set_unused(false);
    size_t sum = 0;
    for (auto x : v)
      sum += popcount(x);
    return sum;
  }

  constexpr bool all() const noexcept {
    set_unused(true);
    return aria::all_of(begin(v), end(v), [](auto x) { return x == ~T(0); });
  }

  constexpr bool any() const noexcept {
    set_unused(false);
    return aria::any_of(begin(v), end(v), [](auto x) { return x != T(0); });
  }

  constexpr bool none() const noexcept {
    set_unused(false);
    return aria::all_of(begin(v), end(v), [](auto x) { return x == T(0); });
  }

  constexpr bool operator[](size_t i) const { return get(i); }
  constexpr reference operator[](size_t i) { return reference(*this, i); }

  constexpr bool test(size_t i) const {
    if (i >= N)
      throw out_of_range("out of range");
    return get(i);
  }

  constexpr size_t size() const noexcept { return N; }

  constexpr unsigned long long to_ullong() const {
    if constexpr (N > bits_per_byte * sizeof(unsigned long long)) {
      throw overflow_error("overflow");
    } else {
      set_unused(false);
      return v[0];
    }
  }

  constexpr bitset operator~() const noexcept { return bitset<N>(*this).flip(); }

  constexpr bitset &operator&=(const bitset &other) noexcept {
    for (int i = 0; i < words; i++)
      v[i] &= other.v[i];
    return *this;
  }

  constexpr bitset &operator|=(const bitset &other) noexcept {
    for (int i = 0; i < words; i++)
      v[i] |= other.v[i];
    return *this;
  }

  constexpr bitset &operator^=(const bitset &other) noexcept {
    for (int i = 0; i < words; i++)
      v[i] ^= other.v[i];
    return *this;
  }

  constexpr bool operator==(const bitset &rhs) const noexcept {
    if (this == &rhs)
      return true;
    set_unused(false);
    rhs.set_unused(false);
    return equal(begin(v), end(v), begin(rhs.v));
  }

private:
  constexpr bool get(size_t i) const {
    auto [words_idx, bit_idx] = get_idx(i);
    return v[words_idx] & (T(1) << bit_idx);
  }

  constexpr void set_unused(bool value) const noexcept {
    static constexpr auto unused_bits = words * bits_per_word - N;
    static constexpr auto used_bits = bits_per_word - unused_bits;
    if constexpr (unused_bits > 0) {
      if (value) {
        v[words - 1] |= (((T(1) << unused_bits) - 1) << used_bits);
      } else {
        v[words - 1] &= ((T(1) << used_bits) - 1);
      }
    }
  }

  constexpr pair<size_t, size_t> get_idx(size_t i) const { return {i / bits_per_word, i % bits_per_word}; }

  using T = unsigned long long;
  static constexpr ptrdiff_t bits_per_byte = 8;
  static constexpr ptrdiff_t bits_per_word = bits_per_byte * sizeof(T);
  static constexpr ptrdiff_t words = N / bits_per_word + (N % bits_per_word != 0 ? 1 : 0);
  mutable T v[words] = {};
};

template <size_t N> constexpr bitset<N> operator&(const bitset<N> &a, const bitset<N> &b) noexcept { return bitset<N>(a) &= b; }
template <size_t N> constexpr bitset<N> operator|(const bitset<N> &a, const bitset<N> &b) noexcept { return bitset<N>(a) |= b; }
template <size_t N> constexpr bitset<N> operator^(const bitset<N> &a, const bitset<N> &b) noexcept { return bitset<N>(a) ^= b; }

} // namespace aria