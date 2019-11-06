# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release

# Include any dependencies generated for this target.
include thread/tests/CMakeFiles/test_multi_thread.dir/depend.make

# Include the progress variables for this target.
include thread/tests/CMakeFiles/test_multi_thread.dir/progress.make

# Include the compile flags for this target's objects.
include thread/tests/CMakeFiles/test_multi_thread.dir/flags.make

thread/tests/CMakeFiles/test_multi_thread.dir/multi_thread.cpp.o: thread/tests/CMakeFiles/test_multi_thread.dir/flags.make
thread/tests/CMakeFiles/test_multi_thread.dir/multi_thread.cpp.o: ../thread/tests/multi_thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object thread/tests/CMakeFiles/test_multi_thread.dir/multi_thread.cpp.o"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_multi_thread.dir/multi_thread.cpp.o -c /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/thread/tests/multi_thread.cpp

thread/tests/CMakeFiles/test_multi_thread.dir/multi_thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_multi_thread.dir/multi_thread.cpp.i"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/thread/tests/multi_thread.cpp > CMakeFiles/test_multi_thread.dir/multi_thread.cpp.i

thread/tests/CMakeFiles/test_multi_thread.dir/multi_thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_multi_thread.dir/multi_thread.cpp.s"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/thread/tests/multi_thread.cpp -o CMakeFiles/test_multi_thread.dir/multi_thread.cpp.s

# Object files for target test_multi_thread
test_multi_thread_OBJECTS = \
"CMakeFiles/test_multi_thread.dir/multi_thread.cpp.o"

# External object files for target test_multi_thread
test_multi_thread_EXTERNAL_OBJECTS =

thread/tests/test_multi_thread: thread/tests/CMakeFiles/test_multi_thread.dir/multi_thread.cpp.o
thread/tests/test_multi_thread: thread/tests/CMakeFiles/test_multi_thread.dir/build.make
thread/tests/test_multi_thread: thread/libthread.so
thread/tests/test_multi_thread: stack/libstack.so
thread/tests/test_multi_thread: thread/tests/CMakeFiles/test_multi_thread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_multi_thread"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_multi_thread.dir/link.txt --verbose=$(VERBOSE)
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests && ./test_multi_thread

# Rule to build all files generated by this target.
thread/tests/CMakeFiles/test_multi_thread.dir/build: thread/tests/test_multi_thread

.PHONY : thread/tests/CMakeFiles/test_multi_thread.dir/build

thread/tests/CMakeFiles/test_multi_thread.dir/clean:
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_multi_thread.dir/cmake_clean.cmake
.PHONY : thread/tests/CMakeFiles/test_multi_thread.dir/clean

thread/tests/CMakeFiles/test_multi_thread.dir/depend:
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/thread/tests /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/release/thread/tests/CMakeFiles/test_multi_thread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thread/tests/CMakeFiles/test_multi_thread.dir/depend

