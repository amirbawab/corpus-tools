project(ngram)
set(NGRAM_EXEC "ngram")

# Include header files
include_directories(include)

# Add ngram exectuable
add_executable(${NGRAM_EXEC} src/ngram.cpp)

# Link xml project library
target_link_libraries(${NGRAM_EXEC} pugixml)