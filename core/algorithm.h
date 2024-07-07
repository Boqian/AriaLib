#pragma once
#include "functional.h"
#include "iterator.h"
#include "ranges.h"
#include "utility.h"

// https://en.cppreference.com/w/cpp/algorithm
namespace aria {

//---------------------Batch operations---------------------
template <input_iterator It, class UnaryFunc> UnaryFunc for_each(It first, It last, UnaryFunc f) {
  for (; first != last; ++first)
    f(*first);
  return f;
}

template <input_iterator It, class Size, class UnaryFunc> It for_each_n(It first, Size n, UnaryFunc f) {
  for (Size i = 0; i < n; ++first, ++i)
    f(*first);
  return first;
}

//---------------------Search operations---------------------
// all_of any_of none_of find, fine_if count count_if equal
template <input_iterator It, class UnaryPred> constexpr bool all_of(It first, It last, UnaryPred p) {
  for (; first != last; ++first) {
    if (!p(*first))
      return false;
  }
  return true;
}

template <input_iterator It, class UnaryPred> constexpr bool any_of(It first, It last, UnaryPred p) {
  for (; first != last; ++first) {
    if (p(*first))
      return true;
  }
  return false;
}

template <input_iterator It, class UnaryPred> constexpr bool none_of(It first, It last, UnaryPred p) { return !any_of(first, last, p); }

template <input_iterator It, class T> constexpr It find(It first, It last, const T &value) {
  for (; first != last && *first != value; ++first) {
  }
  return first;
}

template <input_iterator It, class UnaryPred> constexpr It find_if(It first, It last, UnaryPred p) {
  for (; first != last && !p(*first); ++first) {
  }
  return first;
}

template <input_iterator It, class UnaryPred> constexpr It find_if_not(It first, It last, UnaryPred p) {
  for (; first != last && p(*first); ++first) {
  }
  return first;
}

template <input_iterator It, class T> size_t count(It first, It last, const T &x) {
  size_t cnt = 0;
  for (; first != last; ++first) {
    if (*first == x)
      cnt++;
  }
  return cnt;
}

template <input_iterator It, class UnaryPred> size_t count_if(It first, It last, UnaryPred p) {
  size_t cnt = 0;
  for (; first != last; ++first) {
    if (p(*first))
      cnt++;
  }
  return cnt;
}

//-----------------------Comparison operations-----------------------
template <input_iterator It1, input_iterator It2> constexpr bool equal(It1 first1, It1 last1, It2 first2) {
  for (; first1 != last1; ++first1, ++first2) {
    if (*first1 != *first2)
      return false;
  }
  return true;
}

template <input_iterator It1, input_iterator It2> constexpr bool equal(It1 first1, It1 last1, It2 first2, It2 last2) {
  if (distance(first1, last1) != distance(first2, last2))
    return false;

  return equal(first1, last1, first2);
}

template <input_iterator It1, input_iterator It2, class Cmp>
constexpr auto lexicographical_compare_three_way(It1 first1, It1 last1, It2 first2, It2 last2, Cmp comp)
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

template <input_iterator It1, input_iterator It2>
constexpr auto lexicographical_compare_three_way(It1 first1, It1 last1, It2 first2, It2 last2) {
  return aria::lexicographical_compare_three_way(first1, last1, first2, last2, std::compare_three_way());
}

//-----------------------Minimum/maximum operations-----------------------
template <forward_iterator It, class Compare> constexpr It max_element(It first, It last, Compare cmp) {
  if (first == last)
    return last;

  auto max_it = first;
  for (; first != last; ++first) {
    if (cmp(*max_it, *first))
      max_it = first;
  }
  return max_it;
}

template <forward_iterator It> constexpr It max_element(It first, It last) { return max_element(first, last, aria::less()); }

template <forward_iterator It, class Compare> constexpr It min_element(It first, It last, Compare cmp) {
  if (first == last)
    return last;

  auto min_it = first;
  for (; first != last; ++first) {
    if (cmp(*first, *min_it))
      min_it = first;
  }
  return min_it;
}

template <forward_iterator It> constexpr It min_element(It first, It last) { return min_element(first, last, aria::less()); }

template <class T> constexpr const T &max(const T &a, const T &b) { return (a < b) ? b : a; }
template <class T, class Compare> const T &max(const T &a, const T &b, Compare comp) { return comp(a, b) ? b : a; }
template <class T> constexpr T max(std::initializer_list<T> ilist) { return *max_element(ilist.begin(), ilist.end()); }
template <class T> constexpr const T &min(const T &a, const T &b) { return (a < b) ? a : b; }
template <class T, class Compare> const T &min(const T &a, const T &b, Compare comp) { return comp(a, b) ? a : b; }
template <class T> constexpr T min(std::initializer_list<T> ilist) { return *min_element(ilist.begin(), ilist.end()); }

//-----------------------Modifying sequence operations-----------------------
// copy copy_if copy_n
template <input_iterator InputIt, class OutputIt> constexpr OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
  for (; first != last; ++first, ++d_first)
    *d_first = *first;
  return d_first;
}

template <input_iterator InputIt, class OutputIt, class UnaryPred>
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

template <input_iterator InputIt, class OutputIt, class UnaryOp>
constexpr OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOp unary_op) {
  for (; first1 != last1; ++d_first, ++first1) {
    *d_first = unary_op(*first1);
  }
  return d_first;
}

template <input_iterator InputIt1, input_iterator InputIt2, class OutputIt, class BinaryOp>
constexpr OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, BinaryOp binary_op) {
  for (; first1 != last1; ++d_first, ++first1, ++first2) {
    *d_first = binary_op(*first1, *first2);
  }
  return d_first;
}

template <forward_iterator It, class T> constexpr void fill(It first, It last, const T &value) {
  for (; first != last; ++first)
    *first = value;
}

template <class OutputIt, class Size, class T> constexpr OutputIt fill_n(OutputIt first, Size count, const T &value) {
  for (Size i = 0; i < count; i++)
    *first++ = value;
  return first;
}

//-----------------------Order-changing operations-----------------------
template <forward_iterator It1, forward_iterator It2> constexpr void iter_swap(It1 a, It2 b) {
  using aria::swap;
  swap(*a, *b);
}

template <bidirectional_iterator It> void reverse(It first, It last) {
  if (first == last)
    return;
  --last;
  for (; first != last; ++first, --last)
    iter_swap(first, last);
}

//-----------------------Partitioning operations-----------------------
// is_partitioned partition partition_point
template <input_iterator It, class UnaryPred> bool is_partitioned(It first, It last, UnaryPred p) {
  first = find_if_not(first, last, p);
  return none_of(first, last, p);
}

template <input_iterator It, class UnaryPred> It partition(It first, It last, UnaryPred p) {
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

template <input_iterator It, class UnaryPred> It partition_point(It first, It last, UnaryPred p) {
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
template <forward_iterator It, class T, class Compare> constexpr It lower_bound(It first, It last, const T &value, Compare comp) {
  return partition_point(first, last, [&](const auto &x) { return comp(x, value); });
}

template <forward_iterator It, class T> constexpr It lower_bound(It first, It last, const T &value) {
  return lower_bound(first, last, value, less{});
}

template <forward_iterator It, class T, class Compare> constexpr It upper_bound(It first, It last, const T &value, Compare comp) {
  return partition_point(first, last, [&](const auto &x) { return !comp(value, x); });
}

template <forward_iterator It, class T> constexpr It upper_bound(It first, It last, const T &value) {
  return upper_bound(first, last, value, less{});
}

template <forward_iterator It, class T, class Compare> constexpr pair<It, It> equal_range(It first, It last, const T &value, Compare comp) {
  return make_pair(lower_bound(first, last, value, comp), upper_bound(first, last, value, comp));
}

template <forward_iterator It, class T> constexpr pair<It, It> equal_range(It first, It last, const T &value) {
  return equal_range(first, last, value, less{});
}

template <forward_iterator It, class T, class Compare> bool binary_search(It first, It last, const T &value, Compare comp) {
  const auto it = lower_bound(first, last, value, comp);
  return (it != last and !(comp(value, *it)));
}

template <forward_iterator It, class T> bool binary_search(It first, It last, const T &value) {
  return binary_search(first, last, value, less{});
}

//-----------------------Heap operations-----------------------
namespace _heap {
template <integral I> auto left(I i) { return i * 2 + 1; }
template <integral I> auto right(I i) { return i * 2 + 2; }
template <integral I> auto parent(I i) { return (i - 1) / 2; }
template <integral I> bool is_leaf(I i, I n) { return left(i) >= n; }

template <random_access_iterator It> auto left(It first, iter_difference_t<It> i) { return first + left(i); }
template <random_access_iterator It> auto right(It first, iter_difference_t<It> i) { return first + right(i); }
template <random_access_iterator It> auto parent(It first, iter_difference_t<It> i) { return first + parent(i); }

template <random_access_iterator It, class Compare, class Diff = iter_difference_t<It>>
constexpr void sift_down(It first, iter_difference_t<It> pos, iter_difference_t<It> n, Compare comp) {
  if (is_leaf(pos, n))
    return;
  const auto it = first + pos;
  const auto left_idx = left(pos), right_idx = right(pos);
  bool par_less_left = comp(*it, *left(first, pos)), par_less_right = false, left_less_right = false;

  if (right_idx < n) {
    par_less_right = comp(*it, *right(first, pos));
    left_less_right = comp(*left(first, pos), *right(first, pos));
  }

  if (par_less_left && !left_less_right) {
    iter_swap(it, left(first, pos));
    sift_down(first, left_idx, n, comp);
  } else if (par_less_right && left_less_right) {
    iter_swap(it, right(first, pos));
    sift_down(first, right_idx, n, comp);
  }
}

template <random_access_iterator It, class Compare> constexpr void sift_up(It first, iter_difference_t<It> pos, Compare comp) {
  if (pos == 0)
    return;
  const auto it = first + pos, parent_it = parent(first, pos);
  if (comp(*parent_it, *it)) {
    iter_swap(it, parent_it);
    sift_up(first, parent(pos), comp);
  }
}

template <random_access_iterator It, class Compare>
void make_heap_impl(It first, iter_difference_t<It> pos, iter_difference_t<It> n, Compare comp) {
  if (pos >= n)
    return;
  auto left_idx = left(pos), right_idx = right(pos);
  make_heap_impl(first, left_idx, n, comp);
  make_heap_impl(first, right_idx, n, comp);
  sift_down(first, pos, n, comp);
}

} // namespace _heap

template <random_access_iterator It, class Compare> constexpr bool is_heap(It first, It last, Compare comp) {
  using Diff = iter_difference_t<It>;
  const Diff size = last - first;
  for (Diff i = 1; i < size; i++) {
    if (comp(*(first + _heap::parent(i)), *(first + i)))
      return false;
  }
  return true;
}

template <random_access_iterator It> constexpr bool is_heap(It first, It last) { return is_heap(first, last, less{}); }

template <random_access_iterator It, class Compare> void make_heap(It first, It last, Compare comp) {
  using value_type = decltype(*first);
  static_assert(is_move_assignable_v<value_type> && is_move_constructible_v<value_type>);
  _heap::make_heap_impl(first, 0, last - first, comp);
}

template <random_access_iterator It> void make_heap(It first, It last) { return make_heap(first, last, less{}); }

template <random_access_iterator It, class Compare> void push_heap(It first, It last, Compare comp) {
  const auto n = last - first;
  if (n > 1)
    _heap::sift_up(first, n - 1, comp);
}

template <random_access_iterator It> void push_heap(It first, It last) { push_heap(first, last, less{}); }

template <random_access_iterator It, class Compare> void pop_heap(It first, It last, Compare comp) {
  const auto n = last - first;
  if (n <= 1)
    return;
  iter_swap(first, last - 1);
  _heap::sift_down(first, 0, n - 1, comp);
}

template <random_access_iterator It> void pop_heap(It first, It last) { pop_heap(first, last, less{}); }

} // namespace aria