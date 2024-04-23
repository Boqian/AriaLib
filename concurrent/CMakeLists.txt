# Collect all source files (both .cpp and .h)
file(GLOB SOURCES "*.cpp" "*.h")

# Create a shared library from the collected sources
add_library(concurrent ${SOURCES})  # Change SHARED to STATIC for a static library

include_directories(../core)
add_subdirectory(tests)

