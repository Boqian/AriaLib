# CMakeList.txt : CMake project for AriaLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(AriaLib)
enable_testing()

add_subdirectory(core)
add_subdirectory(concurrent)
add_subdirectory(meta)

# Add source to this project's executable.
add_executable (AriaLib "AriaLib.cpp" "AriaLib.h")

find_package(fmt CONFIG REQUIRED)
target_link_libraries(AriaLib PRIVATE fmt::fmt)
