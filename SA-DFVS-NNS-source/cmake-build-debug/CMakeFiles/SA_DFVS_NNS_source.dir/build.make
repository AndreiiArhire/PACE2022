# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/SA_DFVS_NNS_source.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SA_DFVS_NNS_source.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SA_DFVS_NNS_source.dir/flags.make

CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.obj: CMakeFiles/SA_DFVS_NNS_source.dir/flags.make
CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\SA_DFVS_NNS_source.dir\main.cpp.obj -c C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\main.cpp

CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\main.cpp > CMakeFiles\SA_DFVS_NNS_source.dir\main.cpp.i

CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\main.cpp -o CMakeFiles\SA_DFVS_NNS_source.dir\main.cpp.s

# Object files for target SA_DFVS_NNS_source
SA_DFVS_NNS_source_OBJECTS = \
"CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.obj"

# External object files for target SA_DFVS_NNS_source
SA_DFVS_NNS_source_EXTERNAL_OBJECTS =

SA_DFVS_NNS_source.exe: CMakeFiles/SA_DFVS_NNS_source.dir/main.cpp.obj
SA_DFVS_NNS_source.exe: CMakeFiles/SA_DFVS_NNS_source.dir/build.make
SA_DFVS_NNS_source.exe: CMakeFiles/SA_DFVS_NNS_source.dir/linklibs.rsp
SA_DFVS_NNS_source.exe: CMakeFiles/SA_DFVS_NNS_source.dir/objects1.rsp
SA_DFVS_NNS_source.exe: CMakeFiles/SA_DFVS_NNS_source.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable SA_DFVS_NNS_source.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\SA_DFVS_NNS_source.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SA_DFVS_NNS_source.dir/build: SA_DFVS_NNS_source.exe

.PHONY : CMakeFiles/SA_DFVS_NNS_source.dir/build

CMakeFiles/SA_DFVS_NNS_source.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\SA_DFVS_NNS_source.dir\cmake_clean.cmake
.PHONY : CMakeFiles/SA_DFVS_NNS_source.dir/clean

CMakeFiles/SA_DFVS_NNS_source.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\cmake-build-debug C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\cmake-build-debug C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVS-NNS-source\cmake-build-debug\CMakeFiles\SA_DFVS_NNS_source.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SA_DFVS_NNS_source.dir/depend
