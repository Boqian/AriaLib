include_directories(../)

# GTest
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
  test_synchronized
  test_synchronized.cpp
)
target_link_libraries(
  test_synchronized
  GTest::gtest_main GTest::GTest concurrent
)

gtest_discover_tests(test_synchronized)