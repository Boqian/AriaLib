// AriaLib.cpp : Defines the entry point for the application.
//

#include "AriaLib.h"

using namespace std;

#include <fmt/core.h>
#include <optional>
#include <string>
#include <vector>
#include <iterator>


int main()
{
  std::optional<int> a = 1;

    fmt::print("Hello World! {}\n", a.value());
    return 0;
}
