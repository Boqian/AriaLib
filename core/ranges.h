#pragma once
#include "concepts.h"
#include "iterator.h"

namespace aria {
namespace ranges {

template <class> inline constexpr bool enable_borrowed_range = false;

namespace _begin {
void begin() = delete; // Block unqualified name lookup

template <class T> concept has_member = requires(T t) {
  { t.begin() } -> input_or_output_iterator;
};

template <class T> concept has_adl = has_class_or_enumTpe<T> && requires(T t) {
  { begin(t) } -> input_or_output_iterator;
};

template <class T> concept has_begin = is_array_v<remove_reference_t<T>> || has_member<T> || has_adl<T>;
} // namespace _begin

template <_begin::has_begin T> constexpr auto begin(T &&val) {
  if constexpr (is_array_v<remove_reference_t<T>>) {
    return val;
  } else if constexpr (_begin::has_member<T>) {
    return val.begin();
  } else if constexpr (_begin::has_adl<T>) {
    return begin(val);
  }
}

template <class T> using iterator_t = decltype(ranges::begin(declval<T &>()));

namespace _end {
void end() = delete; // Block unqualified name lookup

template <class T> concept has_member = requires(T t) {
  { t.end() } -> sentinel_for<iterator_t<T>>;
};

template <class T> concept has_adl = has_class_or_enumTpe<T> && requires(T t) {
  { end(t) } -> sentinel_for<iterator_t<T>>;
};

template <class T> concept has_end = is_array_v<remove_reference_t<T>> || has_member<T> || has_adl<T>;
} // namespace _end

template <_end::has_end T> constexpr auto end(T &&val) {
  if constexpr (is_array_v<remove_reference_t<T>>) {
    return val + extent_v<remove_reference_t<T>>;
  } else if constexpr (_end::has_member<T>) {
    return val.end();
  } else if constexpr (_end::has_adl<T>) {
    return end(val);
  }
}

template <class T> concept range = requires(T &t) {
  ranges::begin(t);
  ranges::end(t);
};

template <range R> using range_difference_t = iter_difference_t<iterator_t<R>>;
template <range R> using sentinel_t = decltype(ranges::end(declval<R &>()));
template <range R> using range_value_t = iter_value_t<iterator_t<R>>;
template <range R> using range_reference_t = iter_reference_t<iterator_t<R>>;

template <class Se, class It> concept sized_sentinel_for = sentinel_for<Se, It> && requires(const It &i, const Se &s) {
  { s - i } -> same_as<iter_difference_t<It>>;
  { i - s } -> same_as<iter_difference_t<It>>;
};

namespace _size {
void size() = delete;

template <class T> concept has_member = requires(T t) {
  { t.size() } -> integral;
};

template <class T> concept has_adl = requires(T t) {
  { size(t) } -> integral;
};

template <class T> concept can_difference = requires(T t) {
  { ranges::begin(t) } -> forward_iterator;
  { ranges::end(t) } -> sized_sentinel_for<decltype(ranges::begin(t))>;
};

template <class T> concept has_size = is_array_v<remove_reference_t<T>> || has_member<T> || has_adl<T> || can_difference<T>;

} // namespace _size

template <_size::has_size T> auto size(T &&val) {
  if constexpr (is_array_v<remove_reference_t<T>>) {
    return extent_v<remove_cvref_t<T>>;
  } else if constexpr (_size::has_member<T>) {
    return val.size();
  } else if constexpr (_size::has_adl<T>) {
    return size(val);
  } else if (_size::can_difference<T>) {
    return ranges::end(val) - ranges::begin(val);
  }
}

namespace _data {
template <class T> concept points_to_object = is_pointer_v<T> && is_object_v<remove_pointer_t<T>>;

template <class T> concept has_member = requires(T t) {
  { t.data() } -> points_to_object;
};

template <class T> concept has_contiguous_iterator = requires(T t) {
  { ranges::begin(t) } -> contiguous_iterator;
};

template <class T> concept has_data = has_member<T> || has_contiguous_iterator<T>;

} // namespace _data

template <_data::has_data T> constexpr auto data(T &&val) noexcept {
  if constexpr (_data::has_member<T>) {
    return val.data();
  } else if (_data::has_contiguous_iterator<T>) {
    return to_address(ranges::begin(val));
  }
}

} // namespace ranges
} // namespace aria