cmake_minimum_required(VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Include xml project header files
# Note: Include must be done before add_subdirectory()
include_directories(thirdparty/pugixml/src)

# Compile with -std=c++11
add_compile_options(-std=c++11)

# Add XML parser project
add_subdirectory(thirdparty/pugixml)

# Add ngram project
add_subdirectory(ngram)
