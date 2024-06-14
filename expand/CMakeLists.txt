# Collect all source files (both .cpp and .h)
file(GLOB SOURCES "*.cpp" "*.h")

# Create a shared library from the collected sources
add_library(expand ${SOURCES})  # Change SHARED to STATIC for a static library

set_target_properties(expand PROPERTIES LINKER_LANGUAGE CXX)
include_directories(../core)
add_subdirectory(tests)

