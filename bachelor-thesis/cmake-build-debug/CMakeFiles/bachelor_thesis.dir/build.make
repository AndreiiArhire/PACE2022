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
CMAKE_SOURCE_DIR = C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/bachelor_thesis.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/bachelor_thesis.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bachelor_thesis.dir/flags.make

CMakeFiles/bachelor_thesis.dir/main.cpp.obj: CMakeFiles/bachelor_thesis.dir/flags.make
CMakeFiles/bachelor_thesis.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/bachelor_thesis.dir/main.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\bachelor_thesis.dir\main.cpp.obj -c C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\main.cpp

CMakeFiles/bachelor_thesis.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bachelor_thesis.dir/main.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\main.cpp > CMakeFiles\bachelor_thesis.dir\main.cpp.i

CMakeFiles/bachelor_thesis.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bachelor_thesis.dir/main.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\main.cpp -o CMakeFiles\bachelor_thesis.dir\main.cpp.s

CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.obj: CMakeFiles/bachelor_thesis.dir/flags.make
CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.obj: ../SA__FVSP_NNS.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\bachelor_thesis.dir\SA__FVSP_NNS.cpp.obj -c C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\SA__FVSP_NNS.cpp

CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\SA__FVSP_NNS.cpp > CMakeFiles\bachelor_thesis.dir\SA__FVSP_NNS.cpp.i

CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\SA__FVSP_NNS.cpp -o CMakeFiles\bachelor_thesis.dir\SA__FVSP_NNS.cpp.s

# Object files for target bachelor_thesis
bachelor_thesis_OBJECTS = \
"CMakeFiles/bachelor_thesis.dir/main.cpp.obj" \
"CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.obj"

# External object files for target bachelor_thesis
bachelor_thesis_EXTERNAL_OBJECTS =

bachelor_thesis.exe: CMakeFiles/bachelor_thesis.dir/main.cpp.obj
bachelor_thesis.exe: CMakeFiles/bachelor_thesis.dir/SA__FVSP_NNS.cpp.obj
bachelor_thesis.exe: CMakeFiles/bachelor_thesis.dir/build.make
bachelor_thesis.exe: CMakeFiles/bachelor_thesis.dir/linklibs.rsp
bachelor_thesis.exe: CMakeFiles/bachelor_thesis.dir/objects1.rsp
bachelor_thesis.exe: CMakeFiles/bachelor_thesis.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable bachelor_thesis.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\bachelor_thesis.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bachelor_thesis.dir/build: bachelor_thesis.exe

.PHONY : CMakeFiles/bachelor_thesis.dir/build

CMakeFiles/bachelor_thesis.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\bachelor_thesis.dir\cmake_clean.cmake
.PHONY : CMakeFiles/bachelor_thesis.dir/clean

CMakeFiles/bachelor_thesis.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug C:\Users\andre\OneDrive\Desktop\bachelor-testcases\bachelor-thesis\cmake-build-debug\CMakeFiles\bachelor_thesis.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bachelor_thesis.dir/depend

