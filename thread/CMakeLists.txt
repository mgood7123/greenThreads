cmake_minimum_required(VERSION 3.4.1)

add_library(thread SHARED new.cpp)
add_dependencies(thread stack)
target_link_libraries(thread stack)
add_subdirectory(tests)
