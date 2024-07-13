#pragma once
#include "concepts.h"
#include "utility.h"

namespace aria {

namespace _node_handle {
template <class Key> struct set_base {
public:
  using value_type = const Key;
  template <class Self> value_type &value(this Self const &self) { return self->get_value(); }
};

template <class Key, class T> struct map_base {
public:
  using key_type = const Key;
  using mapped_type = T;

  template <class Self> key_type &key(this Self const &self) { return self->get_value().first; }
  template <class Self> mapped_type &mapped(this Self const &self) { return self->get_value().second; }
};

template <class T> concept node = requires(T t) { t.get_value(); };
template <class T> concept base = is_specialization_of_v<T, set_base> || is_specialization_of_v<T, map_base>;

} // namespace _node_handle

template <_node_handle::node Node, _node_handle::base Base> class node_handle : public Base {
public:
  using pointer = Node *;

  node_handle() = default;
  explicit node_handle(pointer const p) : ptr(p) {}
  virtual ~node_handle() {
    if (ptr)
      delete (ptr); // todo use allocator
  }

  node_handle(const node_handle &) = delete;
  node_handle(node_handle &&rhs) noexcept { swap(rhs); }
  node_handle &operator=(const node_handle &) = delete;
  node_handle &operator=(const node_handle &&rhs) noexcept {
    node_handle(move(rhs)).swap(*this);
    return *this;
  }

  bool empty() const noexcept { return ptr; }
  explicit operator bool() const noexcept { return ptr; }
  void swap(node_handle &rhs) noexcept { aria::swap(ptr, rhs.ptr); }
  pointer release() noexcept { return exchange(ptr, nullptr); }
  pointer get() const noexcept { return ptr; }
  pointer operator->() const noexcept { return ptr; }

private:
  pointer ptr{};
};

} // namespace aria