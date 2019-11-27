cmake_minimum_required(VERSION 3.4.1)

include_directories(include)
add_library(thread_safe SHARED src/thread_safeExceptions.cpp src/Lock.cpp)

add_subdirectory(tests)
