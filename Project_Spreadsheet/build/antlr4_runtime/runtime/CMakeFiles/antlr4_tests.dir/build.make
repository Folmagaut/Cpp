# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.31

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\dev\CMake\bin\cmake.exe

# The command to remove a file.
RM = C:\dev\CMake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build

# Include any dependencies generated for this target.
include antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/compiler_depend.make

# Include the progress variables for this target.
include antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/progress.make

# Include the compile flags for this target's objects.
include antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/flags.make

antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/codegen:
.PHONY : antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/codegen

antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/flags.make
antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/includes_CXX.rsp
antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj: C:/Users/Folm-M/Desktop/Projects/Cpp/Project_Spreadsheet/antlr4_runtime/runtime/tests/Utf8Test.cpp
antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj"
	cd /d C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime && C:\dev\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj -MF CMakeFiles\antlr4_tests.dir\tests\Utf8Test.cpp.obj.d -o CMakeFiles\antlr4_tests.dir\tests\Utf8Test.cpp.obj -c C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\antlr4_runtime\runtime\tests\Utf8Test.cpp

antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.i"
	cd /d C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime && C:\dev\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\antlr4_runtime\runtime\tests\Utf8Test.cpp > CMakeFiles\antlr4_tests.dir\tests\Utf8Test.cpp.i

antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.s"
	cd /d C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime && C:\dev\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\antlr4_runtime\runtime\tests\Utf8Test.cpp -o CMakeFiles\antlr4_tests.dir\tests\Utf8Test.cpp.s

# Object files for target antlr4_tests
antlr4_tests_OBJECTS = \
"CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj"

# External object files for target antlr4_tests
antlr4_tests_EXTERNAL_OBJECTS =

antlr4_runtime/runtime/antlr4_tests.exe: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/tests/Utf8Test.cpp.obj
antlr4_runtime/runtime/antlr4_tests.exe: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/build.make
antlr4_runtime/runtime/antlr4_tests.exe: antlr4_runtime/runtime/libantlr4-runtime-static.a
antlr4_runtime/runtime/antlr4_tests.exe: lib/libgtest_maind.a
antlr4_runtime/runtime/antlr4_tests.exe: lib/libgtestd.a
antlr4_runtime/runtime/antlr4_tests.exe: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/linkLibs.rsp
antlr4_runtime/runtime/antlr4_tests.exe: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/objects1.rsp
antlr4_runtime/runtime/antlr4_tests.exe: antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable antlr4_tests.exe"
	cd /d C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\antlr4_tests.dir\link.txt --verbose=$(VERBOSE)
	cd /d C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime && C:\dev\CMake\bin\cmake.exe -D TEST_TARGET=antlr4_tests -D TEST_EXECUTABLE=C:/Users/Folm-M/Desktop/Projects/Cpp/Project_Spreadsheet/build/antlr4_runtime/runtime/antlr4_tests.exe -D TEST_EXECUTOR= -D TEST_WORKING_DIR=C:/Users/Folm-M/Desktop/Projects/Cpp/Project_Spreadsheet/build/antlr4_runtime/runtime -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX= -D TEST_SUFFIX= -D TEST_FILTER= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=antlr4_tests_TESTS -D CTEST_FILE=C:/Users/Folm-M/Desktop/Projects/Cpp/Project_Spreadsheet/build/antlr4_runtime/runtime/antlr4_tests[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -D TEST_DISCOVERY_EXTRA_ARGS= -D TEST_XML_OUTPUT_DIR= -P C:/dev/CMake/share/cmake-3.31/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/build: antlr4_runtime/runtime/antlr4_tests.exe
.PHONY : antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/build

antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/clean:
	cd /d C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime && $(CMAKE_COMMAND) -P CMakeFiles\antlr4_tests.dir\cmake_clean.cmake
.PHONY : antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/clean

antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\antlr4_runtime\runtime C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime C:\Users\Folm-M\Desktop\Projects\Cpp\Project_Spreadsheet\build\antlr4_runtime\runtime\CMakeFiles\antlr4_tests.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : antlr4_runtime/runtime/CMakeFiles/antlr4_tests.dir/depend

