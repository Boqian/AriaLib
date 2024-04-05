// AriaLib.cpp : Defines the entry point for the application.
//

#include "AriaLib.h"

using namespace std;

#include <fmt/core.h>
#include <optional>
#include <string>
#include <vector>
#include <iterator>
#include <set>
#include <map>
#include <initializer_list>
#include <list>


int main()
{
  std::optional<int> a = 1;

    std::vector<std::pair<int, int>> v = {{1, 2}, {3, 4}};

    fmt::print("Hello World! {}\n", a.value());
    return 0;
}
