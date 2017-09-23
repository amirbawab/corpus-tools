cmake_minimum_required(VERSION 3.5)
project(ngram)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(NGRAM_EXEC "ngram")

# Add XML parser project
add_subdirectory(thirdparty/pugixml)

# Add ngram exectuable
add_executable(${NGRAM_EXEC} src/ngram.cpp)
