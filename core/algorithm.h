#pragma once
#include "functional.h"
#include "iterator.h"
#include "ranges.h"
#include "utility.h"

// https://en.cppreference.com/w/cpp/algorithm
namespace aria {

//---------------------Batch operations---------------------
template <class InputIt, class UnaryFunc> UnaryFunc for_each(InputIt first, InputIt last, UnaryFunc f) {
  for (; first != last; ++first)
    f(*first);
  return f;
}

template <class InputIt, class Size, class UnaryFunc> InputIt for_each_n(InputIt first, Size n, UnaryFunc f) {
  for (Size i = 0; i < n; ++first, ++i)
    f(*first);
  return first;
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
  for (; first != last && *first != value; ++first) {
  }
  return first;
}

template <class InputIt, class UnaryPred> constexpr InputIt find_if(InputIt first, InputIt last, UnaryPred p) {
  for (; first != last && !p(*first); ++first) {
  }
  return first;
}

template <class InputIt, class UnaryPred> constexpr InputIt find_if_not(InputIt first, InputIt last, UnaryPred p) {
  for (; first != last && p(*first); ++first) {
  }
  return first;
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

//-----------------------Comparison operations-----------------------
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

template <class InputIt1, class InputIt2, class Cmp>
constexpr auto lexicographical_compare_three_way(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Cmp comp)
    -> decltype(comp(*first1, *first2)) {
  for (;; ++first1, ++first2) {
    if (first1 == last1) {
      return first2 == last2 ? std::strong_ordering::equal : std::strong_ordering::less;
    } else if (first2 == last2) {
      return std::strong_ordering::greater;
    } else {
      if (auto res = comp(*first1, *first2); res != 0) {
        return res;
      }
    }
  }
}

template <class InputIt1, class InputIt2>
constexpr auto lexicographical_compare_three_way(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
  return aria::lexicographical_compare_three_way(first1, last1, first2, last2, std::compare_three_way());
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
// is_partitioned partition partition_point
template <class InputIt, class UnaryPred> bool is_partitioned(InputIt first, InputIt last, UnaryPred p) {
  first = find_if_not(first, last, p);
  return none_of(first, last, p);
}

template <class InputIt, class UnaryPred> InputIt partition(InputIt first, InputIt last, UnaryPred p) {
  first = find_if_not(first, last, p);
  if (first == last)
    return first;
  for (auto i = next(first); i != last; ++i) {
    if (p(*i)) {
      iter_swap(i, first);
      ++first;
    }
  }
  return first;
}

template <class InputIt, class UnaryPred> InputIt partition_point(InputIt first, InputIt last, UnaryPred p) {
  for (auto d = distance(first, last); d > 0;) {
    auto half = d / 2;
    auto mid = advance(first, half);
    if (p(*mid)) {
      first = next(mid);
      d -= half + 1;
    } else {
      d = half;
    }
  }
  return first;
}

//-----------------------Binary search operations-----------------------
// lower_bound upper_bound equal_range binary_search
template <class ForwardIt, class T, class Compare>
constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T &value, Compare comp) {
  return partition_point(first, last, [&](const auto &x) { return comp(x, value); });
}

template <class ForwardIt, class T> constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T &value) {
  return lower_bound(first, last, value, less{});
}

template <class ForwardIt, class T, class Compare>
constexpr ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T &value, Compare comp) {
  return partition_point(first, last, [&](const auto &x) { return !comp(value, x); });
}

template <class ForwardIt, class T> constexpr ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T &value) {
  return upper_bound(first, last, value, less{});
}

template <class ForwardIt, class T, class Compare>
constexpr pair<ForwardIt, ForwardIt> equal_range(ForwardIt first, ForwardIt last, const T &value, Compare comp) {
  return make_pair(lower_bound(first, last, value, comp), upper_bound(first, last, value, comp));
}

template <class ForwardIt, class T> constexpr pair<ForwardIt, ForwardIt> equal_range(ForwardIt first, ForwardIt last, const T &value) {
  return equal_range(first, last, value, less{});
}

template <class ForwardIt, class T, class Compare> bool binary_search(ForwardIt first, ForwardIt last, const T &value, Compare comp) {
  const auto it = lower_bound(first, last, value, comp);
  return (it != last and !(comp(value, *it)));
}

template <class ForwardIt, class T> bool binary_search(ForwardIt first, ForwardIt last, const T &value) {
  return binary_search(first, last, value, less{});
}

} // namespace aria