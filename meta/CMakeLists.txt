# Collect all source files (both .cpp and .h)
file(GLOB SOURCES "*.cpp" "*.h")

# Create a shared library from the collected sources
add_library(meta ${SOURCES}) 

set_target_properties(meta PROPERTIES LINKER_LANGUAGE CXX)

