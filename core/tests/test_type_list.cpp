#include "type_list.h"
#include "type_traits.h"

using namespace aria;

class A {};
class B {};

using mylist = type_list<int, char, bool, double, float, short, int *, char *>;

static_assert(is_same_v<mylist::head, int>);
static_assert(tuple_size_v<mylist> == 8);

static_assert(first_match_index<double, mylist>() == 3);
static_assert(num_match<double, mylist>() == 1);
static_assert(num_match<bool, type_list<int, char, A, B>, is_convertible>() == 2);

static_assert(is_same_v<nth_type<2, mylist>, bool>);