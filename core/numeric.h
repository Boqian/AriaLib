#pragma once
#include "utility.h" //aria::move

namespace aria {

//---------------------Factor operations---------------------

//---------------------Numeric operations---------------------

template <class InputIt, class T> constexpr T accumulate(InputIt first, InputIt last, T init) {
  for (; first != last; ++first)
    init = move(init) + *first;
  return init;
}

template <class InputIt, class T, class BinaryOp> constexpr T accumulate(InputIt first, InputIt last, T init, BinaryOp op) {
  for (; first != last; ++first)
    init = op(move(init), *first);

  return init;
}

template <class ForwardIt, class T> constexpr void iota(ForwardIt first, ForwardIt last, T value) {
  for (; first != last; ++first, ++value) {
    *first = value;
  }
}

template <class InputIt, class T> constexpr T reduce(InputIt first, InputIt last, T init) {
  for (; first != last; ++first)
    init = move(init) + *first;
  return init;
}

template <class InputIt> constexpr auto reduce(InputIt first, InputIt last) { return reduce(first, last, typename InputIt::value_type{}); }

template <class InputIt, class T, class BinaryOp> constexpr T reduce(InputIt first, InputIt last, T init, BinaryOp op) {
  for (; first != last; ++first)
    init = op(move(init), *first);

  return init;
}

} // namespace aria