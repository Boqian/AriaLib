#pragma once
#include "functional.h"
#include "iterator.h"
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

template <class InputIt, class T> constexpr InputIt find(InputIt first, InputIt last, const T &value) {
  auto it = first;
  for (; it != last && *it != value; ++it) {
  }
  return it;
}

template <class InputIt, class UnaryPred> constexpr InputIt find_if(InputIt first, InputIt last, UnaryPred p) {
  auto it = first;
  for (; it != last && !p(*it); ++it) {
  }
  return it;
}

//-----------------------Minimum/maximum operations-----------------------

template <class ForwardIt, class Compare> constexpr ForwardIt max_element(ForwardIt first, ForwardIt last, Compare cmp) {
  if (first == last)
    return last;

  auto max_it = first;
  for (; first != last; ++first) {
    if (cmp(*max_it, *first))
      max_it = first;
  }
  return max_it;
}

template <class ForwardIt> constexpr ForwardIt max_element(ForwardIt first, ForwardIt last) {
  return max_element(first, last, aria::less());
}

template <class ForwardIt, class Compare> constexpr ForwardIt min_element(ForwardIt first, ForwardIt last, Compare cmp) {
  if (first == last)
    return last;

  auto min_it = first;
  for (; first != last; ++first) {
    if (cmp(*first, *min_it))
      min_it = first;
  }
  return min_it;
}

template <class ForwardIt> constexpr ForwardIt min_element(ForwardIt first, ForwardIt last) {
  return min_element(first, last, aria::less());
}

template <class T> constexpr const T &max(const T &a, const T &b) { return (a < b) ? b : a; }
template <class T, class Compare> const T &max(const T &a, const T &b, Compare comp) { return comp(a, b) ? b : a; }
template <class T> constexpr T max(std::initializer_list<T> ilist) { return *max_element(ilist.begin(), ilist.end()); }
template <class T> constexpr const T &min(const T &a, const T &b) { return (a < b) ? a : b; }
template <class T, class Compare> const T &min(const T &a, const T &b, Compare comp) { return comp(a, b) ? a : b; }
template <class T> constexpr T min(std::initializer_list<T> ilist) { return *min_element(ilist.begin(), ilist.end()); }

} // namespace aria