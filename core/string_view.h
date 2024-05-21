#pragma once

#include "iterator.h"
#include "type_traits.h"

namespace aria {

template <class CharT> class basic_string_view : public iterable {
public:
  using value_type = CharT;
  using pointer = CharT *;
  using const_pointer = const CharT *;
  using reference = CharT &;
  using const_reference = const CharT &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator = array_iterator<CharT>;
  using const_iterator = basic_const_iterator<iterator>;
  using reverse_iterator = aria::reverse_iterator<iterator>;
  using const_reverse_iterator = aria::reverse_iterator<const_iterator>;
  static constexpr size_type npos = -1;

  constexpr basic_string_view() noexcept = default;
  constexpr basic_string_view(const CharT *s, size_type count) noexcept : m_ptr(s), m_size(count) {}
  constexpr basic_string_view(const CharT *s) noexcept : m_ptr(s), m_size(strlen(s)) {}
  constexpr basic_string_view(nullptr_t) = delete;

  // todo use to_address
  template <class It, class End> constexpr basic_string_view(It it, End end) : m_ptr(it.operator->()), m_size(end - it) {}
  template <class R> constexpr basic_string_view(const R &r) : m_ptr(aria::begin(r).operator->()), m_size(aria::end(r) - aria::begin(r)) {}

  constexpr const_reference operator[](size_type i) const noexcept { return *(m_ptr + i); }
  constexpr const_pointer data() const noexcept { return m_ptr; }
  constexpr const_reference front() const noexcept { return *m_ptr; }
  constexpr const_reference back() const noexcept { return *(m_ptr + m_size - 1); }
  constexpr size_type size() const noexcept { return m_size; }
  constexpr size_type length() const noexcept { return m_size; }
  constexpr bool empty() const noexcept { return size() == 0; }
  constexpr void remove_prefix(size_type n) { m_ptr += n; }
  constexpr void remove_suffix(size_type n) { m_size -= n; }
  constexpr auto begin() const noexcept { return const_iterator(m_ptr); }
  constexpr auto end() const noexcept { return const_iterator(m_ptr + m_size); }
  constexpr auto begin() noexcept { return iterator(m_ptr); }
  constexpr auto end() noexcept { return iterator(m_ptr + m_size); }

  constexpr void swap(basic_string_view &rhs) noexcept {
    aria::swap(m_ptr, rhs.m_ptr);
    aria::swap(m_size, rhs.m_size);
  }

  constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const {
    if (pos >= size())
      return {};
    const auto len = min(size() - pos, count);
    return basic_string_view(m_ptr + pos, len);
  }

private:
  const_pointer m_ptr{};
  size_type m_size{};
};

using string_view = basic_string_view<char>;

} // namespace aria