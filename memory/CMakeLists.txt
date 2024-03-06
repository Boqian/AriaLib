# Collect all source files (both .cpp and .h)
file(GLOB SOURCES "*.cpp" "*.h")

# Create a shared library from the collected sources
add_library(memory ${SOURCES})  # Change SHARED to STATIC for a static library

#add_subdirectory(tests)

