# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chiara/dev/cpp/cpu_emulator_6502

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chiara/dev/cpp/cpu_emulator_6502/build

# Include any dependencies generated for this target.
include emulator/CMakeFiles/emulator.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include emulator/CMakeFiles/emulator.dir/compiler_depend.make

# Include the progress variables for this target.
include emulator/CMakeFiles/emulator.dir/progress.make

# Include the compile flags for this target's objects.
include emulator/CMakeFiles/emulator.dir/flags.make

emulator/CMakeFiles/emulator.dir/src/cpu.cpp.o: emulator/CMakeFiles/emulator.dir/flags.make
emulator/CMakeFiles/emulator.dir/src/cpu.cpp.o: /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/cpu.cpp
emulator/CMakeFiles/emulator.dir/src/cpu.cpp.o: emulator/CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiara/dev/cpp/cpu_emulator_6502/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object emulator/CMakeFiles/emulator.dir/src/cpu.cpp.o"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT emulator/CMakeFiles/emulator.dir/src/cpu.cpp.o -MF CMakeFiles/emulator.dir/src/cpu.cpp.o.d -o CMakeFiles/emulator.dir/src/cpu.cpp.o -c /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/cpu.cpp

emulator/CMakeFiles/emulator.dir/src/cpu.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/cpu.cpp.i"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/cpu.cpp > CMakeFiles/emulator.dir/src/cpu.cpp.i

emulator/CMakeFiles/emulator.dir/src/cpu.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/cpu.cpp.s"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/cpu.cpp -o CMakeFiles/emulator.dir/src/cpu.cpp.s

emulator/CMakeFiles/emulator.dir/src/memory.cpp.o: emulator/CMakeFiles/emulator.dir/flags.make
emulator/CMakeFiles/emulator.dir/src/memory.cpp.o: /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/memory.cpp
emulator/CMakeFiles/emulator.dir/src/memory.cpp.o: emulator/CMakeFiles/emulator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiara/dev/cpp/cpu_emulator_6502/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object emulator/CMakeFiles/emulator.dir/src/memory.cpp.o"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT emulator/CMakeFiles/emulator.dir/src/memory.cpp.o -MF CMakeFiles/emulator.dir/src/memory.cpp.o.d -o CMakeFiles/emulator.dir/src/memory.cpp.o -c /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/memory.cpp

emulator/CMakeFiles/emulator.dir/src/memory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/emulator.dir/src/memory.cpp.i"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/memory.cpp > CMakeFiles/emulator.dir/src/memory.cpp.i

emulator/CMakeFiles/emulator.dir/src/memory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/emulator.dir/src/memory.cpp.s"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chiara/dev/cpp/cpu_emulator_6502/emulator/src/memory.cpp -o CMakeFiles/emulator.dir/src/memory.cpp.s

# Object files for target emulator
emulator_OBJECTS = \
"CMakeFiles/emulator.dir/src/cpu.cpp.o" \
"CMakeFiles/emulator.dir/src/memory.cpp.o"

# External object files for target emulator
emulator_EXTERNAL_OBJECTS =

emulator/libemulator.a: emulator/CMakeFiles/emulator.dir/src/cpu.cpp.o
emulator/libemulator.a: emulator/CMakeFiles/emulator.dir/src/memory.cpp.o
emulator/libemulator.a: emulator/CMakeFiles/emulator.dir/build.make
emulator/libemulator.a: emulator/CMakeFiles/emulator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/chiara/dev/cpp/cpu_emulator_6502/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libemulator.a"
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && $(CMAKE_COMMAND) -P CMakeFiles/emulator.dir/cmake_clean_target.cmake
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/emulator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
emulator/CMakeFiles/emulator.dir/build: emulator/libemulator.a
.PHONY : emulator/CMakeFiles/emulator.dir/build

emulator/CMakeFiles/emulator.dir/clean:
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator && $(CMAKE_COMMAND) -P CMakeFiles/emulator.dir/cmake_clean.cmake
.PHONY : emulator/CMakeFiles/emulator.dir/clean

emulator/CMakeFiles/emulator.dir/depend:
	cd /home/chiara/dev/cpp/cpu_emulator_6502/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chiara/dev/cpp/cpu_emulator_6502 /home/chiara/dev/cpp/cpu_emulator_6502/emulator /home/chiara/dev/cpp/cpu_emulator_6502/build /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator /home/chiara/dev/cpp/cpu_emulator_6502/build/emulator/CMakeFiles/emulator.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : emulator/CMakeFiles/emulator.dir/depend
