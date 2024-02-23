# CMakeList.txt : CMake project for AriaLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("AriaLib")

# Add source to this project's executable.
add_executable (AriaLib "AriaLib.cpp" "AriaLib.h" "core/dummy.h" "core/tests/test_dummy.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AriaLib PROPERTY CXX_STANDARD 20)
endif()

enable_testing()

find_package(fmt CONFIG REQUIRED)
#find_package(GTest CONFIG REQUIRED)
target_link_libraries(AriaLib PRIVATE fmt::fmt)
#target_link_libraries(main PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)



#add_test(AllTestsInMain main)