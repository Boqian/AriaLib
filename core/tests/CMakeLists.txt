include_directories(../)

# GTest
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
  test_dummy
  test_dummy.cpp
)
target_link_libraries(
  test_dummy
  GTest::gtest_main GTest::GTest
)

gtest_discover_tests(test_dummy)