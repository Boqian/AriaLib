#pragma once
#include "allocator.h"
#include "utility.h"

template <class T, class Allocator = allocator<T>> class list {
public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using allocator_type = Allocator;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;

  list() = default;

private:
  struct node {
    node() = default;

    pointer ptr = nullptr;
    node *prev = nullptr;
    node *next = nullptr;
  };

  node *first = nullptr;
  node *last = nullptr;
  size_type size = 0;
};