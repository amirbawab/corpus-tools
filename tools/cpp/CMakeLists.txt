cmake_minimum_required(VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include thirdparty libraries header files
# Note: Include must be done before add_subdirectory()
include_directories(thirdparty/pugixml/src)
include_directories(thirdparty/json)

# Compile with -std=c++11
add_compile_options(-std=c++11)

# Add thirdparty projects
add_subdirectory(thirdparty/pugixml)

# Add sub-project
add_subdirectory(ngram)
add_subdirectory(json2xml)
