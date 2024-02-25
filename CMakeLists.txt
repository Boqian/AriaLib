# CMakeList.txt : CMake project for AriaLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("AriaLib")

add_subdirectory(core)
add_subdirectory(concurrent)

# Add source to this project's executable.
add_executable (AriaLib "AriaLib.cpp" "AriaLib.h")

enable_testing()

find_package(fmt CONFIG REQUIRED)
target_link_libraries(AriaLib PRIVATE fmt::fmt)
