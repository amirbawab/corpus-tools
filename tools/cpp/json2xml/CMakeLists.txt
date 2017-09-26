project(json2xml)
set(JSON2XML_EXEC "json2xml")

# Include header files
include_directories(include)

# Add exectuable
add_executable(${JSON2XML_EXEC} src/RedditTree.cpp src/json2xml.cpp)
