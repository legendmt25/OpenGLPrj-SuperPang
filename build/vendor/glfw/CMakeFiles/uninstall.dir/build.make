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


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\trajk\Desktop\glitter\OpenGLPrj

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\trajk\Desktop\glitter\build

# Utility rule file for uninstall.

# Include the progress variables for this target.
include vendor/glfw/CMakeFiles/uninstall.dir/progress.make

vendor/glfw/CMakeFiles/uninstall:
	cd /d C:\Users\trajk\Desktop\glitter\build\vendor\glfw && "C:\Program Files\CMake\bin\cmake.exe" -P C:/Users/trajk/Desktop/glitter/build/vendor/glfw/cmake_uninstall.cmake

uninstall: vendor/glfw/CMakeFiles/uninstall
uninstall: vendor/glfw/CMakeFiles/uninstall.dir/build.make

.PHONY : uninstall

# Rule to build all files generated by this target.
vendor/glfw/CMakeFiles/uninstall.dir/build: uninstall

.PHONY : vendor/glfw/CMakeFiles/uninstall.dir/build

vendor/glfw/CMakeFiles/uninstall.dir/clean:
	cd /d C:\Users\trajk\Desktop\glitter\build\vendor\glfw && $(CMAKE_COMMAND) -P CMakeFiles\uninstall.dir\cmake_clean.cmake
.PHONY : vendor/glfw/CMakeFiles/uninstall.dir/clean

vendor/glfw/CMakeFiles/uninstall.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\trajk\Desktop\glitter\OpenGLPrj C:\Users\trajk\Desktop\glitter\OpenGLPrj\vendor\glfw C:\Users\trajk\Desktop\glitter\build C:\Users\trajk\Desktop\glitter\build\vendor\glfw C:\Users\trajk\Desktop\glitter\build\vendor\glfw\CMakeFiles\uninstall.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : vendor/glfw/CMakeFiles/uninstall.dir/depend
