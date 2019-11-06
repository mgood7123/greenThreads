cmake_minimum_required(VERSION 3.4.1)

include_directories(include)
add_library(atom SHARED src/atomExceptions.cpp)

add_subdirectory(tests)
