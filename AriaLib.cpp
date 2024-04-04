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


int main()
{
  std::optional<int> a = 1;

 // std::vector<int> a;
 // auto it = std::reverse_iterator<std::vector<int>::iterator>();

  std::set<int> s = {1, 2, 3};
  //auto it = s.begin();
  auto it = std::reverse_iterator<std::set<int>::iterator>();
//  it += 2;
  //  auto y = it + 2;

    fmt::print("Hello World! {}\n", a.value());
    return 0;
}
