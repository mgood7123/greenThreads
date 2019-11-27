cmake_minimum_required(VERSION 3.4.1)

include_directories(include)

add_library(thread SHARED src/ExecutionManager.cpp)
add_dependencies(thread stack)
target_link_libraries(thread stack)
#add_subdirectory(tests)
