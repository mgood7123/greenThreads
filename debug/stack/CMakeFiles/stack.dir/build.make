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
CMAKE_SOURCE_DIR = /home/brothercomplex/git/greenThreads

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/brothercomplex/git/greenThreads/debug

# Include any dependencies generated for this target.
include stack/CMakeFiles/stack.dir/depend.make

# Include the progress variables for this target.
include stack/CMakeFiles/stack.dir/progress.make

# Include the compile flags for this target's objects.
include stack/CMakeFiles/stack.dir/flags.make

stack/CMakeFiles/stack.dir/direction.cpp.o: stack/CMakeFiles/stack.dir/flags.make
stack/CMakeFiles/stack.dir/direction.cpp.o: ../stack/direction.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/brothercomplex/git/greenThreads/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object stack/CMakeFiles/stack.dir/direction.cpp.o"
	cd /home/brothercomplex/git/greenThreads/debug/stack && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/stack.dir/direction.cpp.o -c /home/brothercomplex/git/greenThreads/stack/direction.cpp

stack/CMakeFiles/stack.dir/direction.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stack.dir/direction.cpp.i"
	cd /home/brothercomplex/git/greenThreads/debug/stack && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/brothercomplex/git/greenThreads/stack/direction.cpp > CMakeFiles/stack.dir/direction.cpp.i

stack/CMakeFiles/stack.dir/direction.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stack.dir/direction.cpp.s"
	cd /home/brothercomplex/git/greenThreads/debug/stack && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/brothercomplex/git/greenThreads/stack/direction.cpp -o CMakeFiles/stack.dir/direction.cpp.s

stack/CMakeFiles/stack.dir/new.cpp.o: stack/CMakeFiles/stack.dir/flags.make
stack/CMakeFiles/stack.dir/new.cpp.o: ../stack/new.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/brothercomplex/git/greenThreads/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object stack/CMakeFiles/stack.dir/new.cpp.o"
	cd /home/brothercomplex/git/greenThreads/debug/stack && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/stack.dir/new.cpp.o -c /home/brothercomplex/git/greenThreads/stack/new.cpp

stack/CMakeFiles/stack.dir/new.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stack.dir/new.cpp.i"
	cd /home/brothercomplex/git/greenThreads/debug/stack && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/brothercomplex/git/greenThreads/stack/new.cpp > CMakeFiles/stack.dir/new.cpp.i

stack/CMakeFiles/stack.dir/new.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stack.dir/new.cpp.s"
	cd /home/brothercomplex/git/greenThreads/debug/stack && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/brothercomplex/git/greenThreads/stack/new.cpp -o CMakeFiles/stack.dir/new.cpp.s

# Object files for target stack
stack_OBJECTS = \
"CMakeFiles/stack.dir/direction.cpp.o" \
"CMakeFiles/stack.dir/new.cpp.o"

# External object files for target stack
stack_EXTERNAL_OBJECTS =

stack/libstack.so: stack/CMakeFiles/stack.dir/direction.cpp.o
stack/libstack.so: stack/CMakeFiles/stack.dir/new.cpp.o
stack/libstack.so: stack/CMakeFiles/stack.dir/build.make
stack/libstack.so: stack/CMakeFiles/stack.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/brothercomplex/git/greenThreads/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libstack.so"
	cd /home/brothercomplex/git/greenThreads/debug/stack && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/stack.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
stack/CMakeFiles/stack.dir/build: stack/libstack.so

.PHONY : stack/CMakeFiles/stack.dir/build

stack/CMakeFiles/stack.dir/clean:
	cd /home/brothercomplex/git/greenThreads/debug/stack && $(CMAKE_COMMAND) -P CMakeFiles/stack.dir/cmake_clean.cmake
.PHONY : stack/CMakeFiles/stack.dir/clean

stack/CMakeFiles/stack.dir/depend:
	cd /home/brothercomplex/git/greenThreads/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/brothercomplex/git/greenThreads /home/brothercomplex/git/greenThreads/stack /home/brothercomplex/git/greenThreads/debug /home/brothercomplex/git/greenThreads/debug/stack /home/brothercomplex/git/greenThreads/debug/stack/CMakeFiles/stack.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : stack/CMakeFiles/stack.dir/depend
