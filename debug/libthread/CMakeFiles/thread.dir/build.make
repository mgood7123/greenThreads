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
CMAKE_BINARY_DIR = /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug

# Include any dependencies generated for this target.
include libthread/CMakeFiles/thread.dir/depend.make

# Include the progress variables for this target.
include libthread/CMakeFiles/thread.dir/progress.make

# Include the compile flags for this target's objects.
include libthread/CMakeFiles/thread.dir/flags.make

libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o: libthread/CMakeFiles/thread.dir/flags.make
libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o: ../libthread/src/ExecutionManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/thread.dir/src/ExecutionManager.cpp.o -c /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/libthread/src/ExecutionManager.cpp

libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/thread.dir/src/ExecutionManager.cpp.i"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/libthread/src/ExecutionManager.cpp > CMakeFiles/thread.dir/src/ExecutionManager.cpp.i

libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/thread.dir/src/ExecutionManager.cpp.s"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/libthread/src/ExecutionManager.cpp -o CMakeFiles/thread.dir/src/ExecutionManager.cpp.s

# Object files for target thread
thread_OBJECTS = \
"CMakeFiles/thread.dir/src/ExecutionManager.cpp.o"

# External object files for target thread
thread_EXTERNAL_OBJECTS =

libthread/libthread.so: libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o
libthread/libthread.so: libthread/CMakeFiles/thread.dir/build.make
libthread/libthread.so: libstack/libstack.so
libthread/libthread.so: libthread/CMakeFiles/thread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libthread.so"
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/thread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libthread/CMakeFiles/thread.dir/build: libthread/libthread.so

.PHONY : libthread/CMakeFiles/thread.dir/build

libthread/CMakeFiles/thread.dir/clean:
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread && $(CMAKE_COMMAND) -P CMakeFiles/thread.dir/cmake_clean.cmake
.PHONY : libthread/CMakeFiles/thread.dir/clean

libthread/CMakeFiles/thread.dir/depend:
	cd /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/libthread /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread /home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/debug/libthread/CMakeFiles/thread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libthread/CMakeFiles/thread.dir/depend

