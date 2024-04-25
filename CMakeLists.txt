# CMakeList.txt : CMake project for AriaLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

project(AriaLib)

enable_testing()

add_subdirectory(core)
add_subdirectory(concurrent)

# Add source to this project's executable.
add_executable (AriaLib "AriaLib.cpp" "AriaLib.h")

find_package(fmt CONFIG REQUIRED)
target_link_libraries(AriaLib PRIVATE fmt::fmt)
