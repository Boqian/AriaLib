#include <utility>

#include "vector.h"
#include "gtest/gtest.h"

using namespace aria;

TEST(test_vector, base) { vector<int> v(2, 100);
  EXPECT_EQ(v[0], 100);

    EXPECT_EQ(v[1], 100);


}
