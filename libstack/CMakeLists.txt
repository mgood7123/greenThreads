cmake_minimum_required(VERSION 3.4.1)

include_directories(include)

add_library(stack SHARED ../libatom/src/atomExceptions.cpp src/direction.cpp src/new.cpp)
add_subdirectory(tests)
