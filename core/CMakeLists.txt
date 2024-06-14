 

file(GLOB SOURCES "*.cpp" "*.h") 
add_library(core ${SOURCES})   
 

set_target_properties(core PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(tests)