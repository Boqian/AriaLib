#pragma once
#include "utility.h"

namespace aria {

	
template <class ForwardIt1, class ForwardIt2> constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b) { aria::swap(*a, *b); }

template <class Iter> void reverse(Iter first, Iter last) {
  if (first == last)
    return;
  --last;
  for (; first != last; ++first, --last)
    iter_swap(first, last);
}



}