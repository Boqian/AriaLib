include_directories(../)

# GTest
find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB TEST_SOURCES "test_*.cpp")

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} GTest::gtest_main GTest::GTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()
