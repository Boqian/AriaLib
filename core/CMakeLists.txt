# Collect all source files (both .cpp and .h)
file(GLOB SOURCES "*.cpp" "*.h")

# Create a shared library from the collected sources
add_library(core ${SOURCES})  # Change SHARED to STATIC for a static library
#target_include_directories(memory PRIVATE  ${CMAKE_SOURCE_DIR}/meta)

add_subdirectory(tests)