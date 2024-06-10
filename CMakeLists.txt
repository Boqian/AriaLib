# CMakeList.txt : CMake project for AriaLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

project(AriaLib)

enable_testing()

add_subdirectory(core)
add_subdirectory(concurrent)
add_subdirectory(expand)

find_package(fmt CONFIG REQUIRED)

