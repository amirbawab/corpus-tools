cmake_minimum_required(VERSION 3.5)
project(ngram)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(NGRAM_EXEC "ngram")

# Compile with -std=c++11
add_compile_options(-std=c++11)

# Add XML parser project
add_subdirectory(thirdparty/pugixml)

# Include xml project header files
include_directories(thirdparty/pugixml/src)

# Add ngram exectuable
add_executable(${NGRAM_EXEC} src/ngram.cpp)

# Link xml project library
target_link_libraries(${NGRAM_EXEC} pugixml)