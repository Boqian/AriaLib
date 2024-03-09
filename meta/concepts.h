#pragma once
#include "type_traits.h"
#include <utility> //std::declval


namespace aria {
/*
https://en.cppreference.com/w/cpp/header/concepts
*/
template <class T, class U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <class From, class To>
concept convertable_to = requires { static_cast<To>(std::declval<From>()); };


}