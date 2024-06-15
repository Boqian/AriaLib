#pragma once
#include "concepts.h"
#include "iterator.h"

namespace aria {
namespace ranges {

template <class> inline constexpr bool enable_borrowed_range = false;

namespace _begin {
void begin() = delete; // Block unqualified name lookup

template <class T>
concept has_member = requires(T t) {
  { t.begin() } -> input_or_output_iterator;
};

template <class T>
concept has_adl = has_class_or_enumTpe<T> && requires(T t) {
  { begin(t) } -> input_or_output_iterator;
};

template <class T>
concept has_begin = is_array_v<remove_reference_t<T>> || has_member<T> || has_adl<T>;
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

template <class T>
concept has_member = requires(T t) {
  { t.end() } -> sentinel_for<iterator_t<T>>;
};

template <class T>
concept has_adl = has_class_or_enumTpe<T> && requires(T t) {
  { end(t) } -> sentinel_for<iterator_t<T>>;
};

template <class T>
concept has_end = is_array_v<remove_reference_t<T>> || has_member<T> || has_adl<T>;
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

} // namespace ranges
} // namespace aria