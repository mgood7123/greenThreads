cmake_minimum_required(VERSION 3.4.1)

add_executable(test_single_thread single_thread.cpp)
target_link_libraries(test_single_thread thread)

# gdb --quiet -ex "handle SIGCONT pass print nostop" -ex "handle SIGSTOP pass print nostop" -ex "set follow-fork-mode child" -ex r

add_custom_command( TARGET test_single_thread POST_BUILD COMMAND test_single_thread)
add_executable(test_multi_thread multi_thread.cpp)
target_link_libraries(test_multi_thread  thread)
add_custom_command( TARGET test_multi_thread POST_BUILD COMMAND test_multi_thread)
