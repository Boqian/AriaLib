#pragma once
#include "functional.h"
#include "iterator.h"
#include "utility.h"

// https://en.cppreference.com/w/cpp/algorithm
namespace aria {

//---------------------Batch operations---------------------
template <class InputIt, class UnaryFunc> UnaryFunc for_each(InputIt first, InputIt last, UnaryFunc f) {
  for (; first != last; ++first)
    f(*first);
  return f;
}

//---------------------Search operations---------------------
template <class InputIt, class UnaryPred> constexpr bool all_of(InputIt first, InputIt last, UnaryPred p) {
  for (; first != last; ++first) {
    if (!p(*first))
      return false;
  }
  return true;
}

template <class InputIt, class UnaryPred> constexpr bool any_of(InputIt first, InputIt last, UnaryPred p) {
  for (; first != last; ++first) {
    if (p(*first))
      return true;
  }
  return false;
}

template <class InputIt, class UnaryPred> constexpr bool none_of(InputIt first, InputIt last, UnaryPred p) {
  return !any_of(first, last, p);
}

template <class ForwardIt1, class ForwardIt2> constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b) { aria::swap(*a, *b); }

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

template <class InputIt, class T> size_t count(InputIt first, InputIt last, const T &x) {
  size_t cnt = 0;
  for (; first != last; ++first) {
    if (*first == x)
      cnt++;
  }
  return cnt;
}

template <class InputIt, class UnaryPred> size_t count_if(InputIt first, InputIt last, UnaryPred p) {
  size_t cnt = 0;
  for (; first != last; ++first) {
    if (p(*first))
      cnt++;
  }
  return cnt;
}

template <class InputIt1, class InputIt2> constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
  for (; first1 != last1; ++first1, ++first2) {
    if (*first1 != *first2)
      return false;
  }
  return true;
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

//-----------------------Order-changing operations-----------------------
template <class Iter> void reverse(Iter first, Iter last) {
  if (first == last)
    return;
  --last;
  for (; first != last; ++first, --last)
    iter_swap(first, last);
}

} // namespace aria