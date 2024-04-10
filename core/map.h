#pragma once

namespace aria {

namespace detail {
struct tree_node_base {
  tree_node_base *parent{};
  tree_node_base *left{};
  tree_node_base *right{};
};

} // namespace detail

} // namespace aria