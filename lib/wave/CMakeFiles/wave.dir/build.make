# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib

# Include any dependencies generated for this target.
include lib/wave/CMakeFiles/wave.dir/depend.make

# Include the progress variables for this target.
include lib/wave/CMakeFiles/wave.dir/progress.make

# Include the compile flags for this target's objects.
include lib/wave/CMakeFiles/wave.dir/flags.make

lib/wave/CMakeFiles/wave.dir/wave.cpp.o: lib/wave/CMakeFiles/wave.dir/flags.make
lib/wave/CMakeFiles/wave.dir/wave.cpp.o: lib/wave/wave.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object lib/wave/CMakeFiles/wave.dir/wave.cpp.o"
	cd /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/wave.dir/wave.cpp.o -c /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave/wave.cpp

lib/wave/CMakeFiles/wave.dir/wave.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wave.dir/wave.cpp.i"
	cd /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave/wave.cpp > CMakeFiles/wave.dir/wave.cpp.i

lib/wave/CMakeFiles/wave.dir/wave.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wave.dir/wave.cpp.s"
	cd /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave/wave.cpp -o CMakeFiles/wave.dir/wave.cpp.s

lib/wave/CMakeFiles/wave.dir/wave.cpp.o.requires:
.PHONY : lib/wave/CMakeFiles/wave.dir/wave.cpp.o.requires

lib/wave/CMakeFiles/wave.dir/wave.cpp.o.provides: lib/wave/CMakeFiles/wave.dir/wave.cpp.o.requires
	$(MAKE) -f lib/wave/CMakeFiles/wave.dir/build.make lib/wave/CMakeFiles/wave.dir/wave.cpp.o.provides.build
.PHONY : lib/wave/CMakeFiles/wave.dir/wave.cpp.o.provides

lib/wave/CMakeFiles/wave.dir/wave.cpp.o.provides.build: lib/wave/CMakeFiles/wave.dir/wave.cpp.o
.PHONY : lib/wave/CMakeFiles/wave.dir/wave.cpp.o.provides.build

# Object files for target wave
wave_OBJECTS = \
"CMakeFiles/wave.dir/wave.cpp.o"

# External object files for target wave
wave_EXTERNAL_OBJECTS =

lib/libwave.so: lib/wave/CMakeFiles/wave.dir/wave.cpp.o
lib/libwave.so: lib/wave/CMakeFiles/wave.dir/build.make
lib/libwave.so: lib/wave/CMakeFiles/wave.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library ../libwave.so"
	cd /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wave.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/wave/CMakeFiles/wave.dir/build: lib/libwave.so
.PHONY : lib/wave/CMakeFiles/wave.dir/build

lib/wave/CMakeFiles/wave.dir/requires: lib/wave/CMakeFiles/wave.dir/wave.cpp.o.requires
.PHONY : lib/wave/CMakeFiles/wave.dir/requires

lib/wave/CMakeFiles/wave.dir/clean:
	cd /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave && $(CMAKE_COMMAND) -P CMakeFiles/wave.dir/cmake_clean.cmake
.PHONY : lib/wave/CMakeFiles/wave.dir/clean

lib/wave/CMakeFiles/wave.dir/depend:
	cd /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/wave/CMakeFiles/wave.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/wave/CMakeFiles/wave.dir/depend

