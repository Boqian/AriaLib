#include "shared_ptr.h"
#include "gtest/gtest.h"
#include <utility>

using namespace aria;
TEST(test_shared_ptr, basic) {
  { auto p = shared_ptr<int>();
	
	EXPECT_FALSE(p);
	}
}
