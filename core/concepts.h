#pragma once
#include "type_traits.h"

namespace aria {
/*
https://en.cppreference.com/w/cpp/header/concepts
*/
template <class T, class U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <class From, class To>
concept convertible_to = is_convertible_v<From, To> && requires { static_cast<To>(declval<From>()); };

} // namespace aria