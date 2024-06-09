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
// all_of any_of none_of find, fine_if count count_if equal
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

template <class InputIt1, class InputIt2> constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
  if (distance(first1, last1) != distance(first2, last2))
    return false;

  return equal(first1, last1, first2);
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

//-----------------------Modifying sequence operations-----------------------
// copy copy_if copy_n
template <class InputIt, class OutputIt> constexpr OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
  for (; first != last; ++first, ++d_first)
    *d_first = *first;
  return d_first;
}

template <class InputIt, class OutputIt, class UnaryPred>
constexpr OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPred pred) {
  for (; first != last; ++first)
    if (pred(*first)) {
      *d_first = *first;
      ++d_first;
    }
  return d_first;
}

template <class InputIt, class Size, class OutputIt> constexpr OutputIt copy_n(InputIt first, Size count, OutputIt d_first) {
  for (; count > 0; ++first, ++d_first, --count) {
    *d_first = *first;
  }
  return d_first;
}

template <class InputIt, class OutputIt, class UnaryOp>
constexpr OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOp unary_op) {
  for (; first1 != last1; ++d_first, ++first1) {
    *d_first = unary_op(*first1);
  }
  return d_first;
}

template <class InputIt1, class InputIt2, class OutputIt, class BinaryOp>
constexpr OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, BinaryOp binary_op) {
  for (; first1 != last1; ++d_first, ++first1, ++first2) {
    *d_first = binary_op(*first1, *first2);
  }
  return d_first;
}

template <class ForwardIt, class T> constexpr void fill(ForwardIt first, ForwardIt last, const T &value) {
  for (; first != last; ++first)
    *first = value;
}

template <class OutputIt, class Size, class T> constexpr OutputIt fill_n(OutputIt first, Size count, const T &value) {
  for (Size i = 0; i < count; i++)
    *first++ = value;
  return first;
}

//-----------------------Order-changing operations-----------------------
template <class ForwardIt1, class ForwardIt2> constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b) {
  using aria::swap;
  swap(*a, *b);
}

template <class Iter> void reverse(Iter first, Iter last) {
  if (first == last)
    return;
  --last;
  for (; first != last; ++first, --last)
    iter_swap(first, last);
}

//-----------------------Partitioning operations-----------------------
template <class InputIt, class UnaryPred> bool is_partitioned(InputIt first, InputIt last, UnaryPred p) {
  for (; first != last && p(*first); ++first) {
  }
  return none_of(first, last, p);
}

} // namespace aria