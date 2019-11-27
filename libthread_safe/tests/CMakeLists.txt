cmake_minimum_required(VERSION 3.4.1)

# googletest requires 11, thread_safe requires 17

 set(CMAKE_CXX_STANDARD 17) # this errors on ndk
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++17 -fexceptions -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++17 -fexceptions -g0 -O3")

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

# include current directory

include_directories(../include/)
#include_directories(../../libatom/include/)
#include_directories(../../libstack/include/)

# Now simply link against gtest or gtest_main as needed. Eg

#[[
[19:44] <macroprep> can cmake define macros
[19:47] <macroprep> like, COMPILE_AND_STORE_EXECUTABLE(file_name, executable_name, directory)
[19:54] <markand> macroprep, yes with function() or macro()
[19:55] <markand> function are preferred since they keep local variables
[20:16] <macroprep> oh ok
]]

add_executable(
        thread_safe_tests

        ../src/Lock.cpp
        #            ../../libstack/src/new.cpp
        #            ../../libstack/src/direction.cpp
        thread_safe_lock.cpp

        ../src/thread_safeExceptions.cpp
            thread_safe_general.cpp
            thread_safe_thread.cpp
)

target_link_libraries(thread_safe_tests pthread gtest_main)

add_custom_command( TARGET thread_safe_tests POST_BUILD COMMAND mv thread_safe_tests thread_safe_EXECUTABLES/thread_safe_tests)
