# CMakeList.txt : CMake project for AriaLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MDd")

project(AriaLib)
enable_testing()

add_subdirectory(core)
add_subdirectory(concurrent)
add_subdirectory(meta)

# Add source to this project's executable.
add_executable (AriaLib "AriaLib.cpp" "AriaLib.h" "core/allocator.cpp" "core/allocator.h" "core/shared_ptr.h" "core/unique_ptr.h" "core/vector.h")

find_package(fmt CONFIG REQUIRED)
target_link_libraries(AriaLib PRIVATE fmt::fmt)
