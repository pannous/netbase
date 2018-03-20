# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = cmake

# The command to remove a file.
RM = cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = ~/netbase

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = ~/netbase

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start ~/netbase/CMakeFiles ~/netbase/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start ~/netbase/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named netbase

# Build rule for target.
netbase: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 netbase
.PHONY : netbase

# fast build rule for target.
netbase/fast:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/build
.PHONY : netbase/fast

src/console.o: src/console.cpp.o

.PHONY : src/console.o

# target to build an object file
src/console.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/console.cpp.o
.PHONY : src/console.cpp.o

src/console.i: src/console.cpp.i

.PHONY : src/console.i

# target to preprocess a source file
src/console.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/console.cpp.i
.PHONY : src/console.cpp.i

src/console.s: src/console.cpp.s

.PHONY : src/console.s

# target to generate assembly for a file
src/console.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/console.cpp.s
.PHONY : src/console.cpp.s

src/export.o: src/export.cpp.o

.PHONY : src/export.o

# target to build an object file
src/export.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/export.cpp.o
.PHONY : src/export.cpp.o

src/export.i: src/export.cpp.i

.PHONY : src/export.i

# target to preprocess a source file
src/export.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/export.cpp.i
.PHONY : src/export.cpp.i

src/export.s: src/export.cpp.s

.PHONY : src/export.s

# target to generate assembly for a file
src/export.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/export.cpp.s
.PHONY : src/export.cpp.s

src/import.o: src/import.cpp.o

.PHONY : src/import.o

# target to build an object file
src/import.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/import.cpp.o
.PHONY : src/import.cpp.o

src/import.i: src/import.cpp.i

.PHONY : src/import.i

# target to preprocess a source file
src/import.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/import.cpp.i
.PHONY : src/import.cpp.i

src/import.s: src/import.cpp.s

.PHONY : src/import.s

# target to generate assembly for a file
src/import.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/import.cpp.s
.PHONY : src/import.cpp.s

src/init.o: src/init.cpp.o

.PHONY : src/init.o

# target to build an object file
src/init.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/init.cpp.o
.PHONY : src/init.cpp.o

src/init.i: src/init.cpp.i

.PHONY : src/init.i

# target to preprocess a source file
src/init.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/init.cpp.i
.PHONY : src/init.cpp.i

src/init.s: src/init.cpp.s

.PHONY : src/init.s

# target to generate assembly for a file
src/init.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/init.cpp.s
.PHONY : src/init.cpp.s

src/md5.o: src/md5.cpp.o

.PHONY : src/md5.o

# target to build an object file
src/md5.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/md5.cpp.o
.PHONY : src/md5.cpp.o

src/md5.i: src/md5.cpp.i

.PHONY : src/md5.i

# target to preprocess a source file
src/md5.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/md5.cpp.i
.PHONY : src/md5.cpp.i

src/md5.s: src/md5.cpp.s

.PHONY : src/md5.s

# target to generate assembly for a file
src/md5.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/md5.cpp.s
.PHONY : src/md5.cpp.s

src/netbase.o: src/netbase.cpp.o

.PHONY : src/netbase.o

# target to build an object file
src/netbase.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/netbase.cpp.o
.PHONY : src/netbase.cpp.o

src/netbase.i: src/netbase.cpp.i

.PHONY : src/netbase.i

# target to preprocess a source file
src/netbase.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/netbase.cpp.i
.PHONY : src/netbase.cpp.i

src/netbase.s: src/netbase.cpp.s

.PHONY : src/netbase.s

# target to generate assembly for a file
src/netbase.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/netbase.cpp.s
.PHONY : src/netbase.cpp.s

src/query.o: src/query.cpp.o

.PHONY : src/query.o

# target to build an object file
src/query.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/query.cpp.o
.PHONY : src/query.cpp.o

src/query.i: src/query.cpp.i

.PHONY : src/query.i

# target to preprocess a source file
src/query.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/query.cpp.i
.PHONY : src/query.cpp.i

src/query.s: src/query.cpp.s

.PHONY : src/query.s

# target to generate assembly for a file
src/query.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/query.cpp.s
.PHONY : src/query.cpp.s

src/reflection.o: src/reflection.cpp.o

.PHONY : src/reflection.o

# target to build an object file
src/reflection.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/reflection.cpp.o
.PHONY : src/reflection.cpp.o

src/reflection.i: src/reflection.cpp.i

.PHONY : src/reflection.i

# target to preprocess a source file
src/reflection.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/reflection.cpp.i
.PHONY : src/reflection.cpp.i

src/reflection.s: src/reflection.cpp.s

.PHONY : src/reflection.s

# target to generate assembly for a file
src/reflection.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/reflection.cpp.s
.PHONY : src/reflection.cpp.s

src/relations.o: src/relations.cpp.o

.PHONY : src/relations.o

# target to build an object file
src/relations.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/relations.cpp.o
.PHONY : src/relations.cpp.o

src/relations.i: src/relations.cpp.i

.PHONY : src/relations.i

# target to preprocess a source file
src/relations.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/relations.cpp.i
.PHONY : src/relations.cpp.i

src/relations.s: src/relations.cpp.s

.PHONY : src/relations.s

# target to generate assembly for a file
src/relations.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/relations.cpp.s
.PHONY : src/relations.cpp.s

src/shared_map.o: src/shared_map.cpp.o

.PHONY : src/shared_map.o

# target to build an object file
src/shared_map.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/shared_map.cpp.o
.PHONY : src/shared_map.cpp.o

src/shared_map.i: src/shared_map.cpp.i

.PHONY : src/shared_map.i

# target to preprocess a source file
src/shared_map.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/shared_map.cpp.i
.PHONY : src/shared_map.cpp.i

src/shared_map.s: src/shared_map.cpp.s

.PHONY : src/shared_map.s

# target to generate assembly for a file
src/shared_map.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/shared_map.cpp.s
.PHONY : src/shared_map.cpp.s

src/tests.o: src/tests.cpp.o

.PHONY : src/tests.o

# target to build an object file
src/tests.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/tests.cpp.o
.PHONY : src/tests.cpp.o

src/tests.i: src/tests.cpp.i

.PHONY : src/tests.i

# target to preprocess a source file
src/tests.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/tests.cpp.i
.PHONY : src/tests.cpp.i

src/tests.s: src/tests.cpp.s

.PHONY : src/tests.s

# target to generate assembly for a file
src/tests.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/tests.cpp.s
.PHONY : src/tests.cpp.s

src/utf8.o: src/utf8.cpp.o

.PHONY : src/utf8.o

# target to build an object file
src/utf8.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.cpp.o
.PHONY : src/utf8.cpp.o

src/utf8.i: src/utf8.cpp.i

.PHONY : src/utf8.i

# target to preprocess a source file
src/utf8.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.cpp.i
.PHONY : src/utf8.cpp.i

src/utf8.s: src/utf8.cpp.s

.PHONY : src/utf8.s

# target to generate assembly for a file
src/utf8.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.cpp.s
.PHONY : src/utf8.cpp.s

src/util.o: src/util.cpp.o

.PHONY : src/util.o

# target to build an object file
src/util.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/util.cpp.o
.PHONY : src/util.cpp.o

src/util.i: src/util.cpp.i

.PHONY : src/util.i

# target to preprocess a source file
src/util.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/util.cpp.i
.PHONY : src/util.cpp.i

src/util.s: src/util.cpp.s

.PHONY : src/util.s

# target to generate assembly for a file
src/util.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/util.cpp.s
.PHONY : src/util.cpp.s

src/webserver.o: src/webserver.cpp.o

.PHONY : src/webserver.o

# target to build an object file
src/webserver.cpp.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/webserver.cpp.o
.PHONY : src/webserver.cpp.o

src/webserver.i: src/webserver.cpp.i

.PHONY : src/webserver.i

# target to preprocess a source file
src/webserver.cpp.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/webserver.cpp.i
.PHONY : src/webserver.cpp.i

src/webserver.s: src/webserver.cpp.s

.PHONY : src/webserver.s

# target to generate assembly for a file
src/webserver.cpp.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/webserver.cpp.s
.PHONY : src/webserver.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... netbase"
	@echo "... src/console.o"
	@echo "... src/console.i"
	@echo "... src/console.s"
	@echo "... src/export.o"
	@echo "... src/export.i"
	@echo "... src/export.s"
	@echo "... src/import.o"
	@echo "... src/import.i"
	@echo "... src/import.s"
	@echo "... src/init.o"
	@echo "... src/init.i"
	@echo "... src/init.s"
	@echo "... src/md5.o"
	@echo "... src/md5.i"
	@echo "... src/md5.s"
	@echo "... src/netbase.o"
	@echo "... src/netbase.i"
	@echo "... src/netbase.s"
	@echo "... src/query.o"
	@echo "... src/query.i"
	@echo "... src/query.s"
	@echo "... src/reflection.o"
	@echo "... src/reflection.i"
	@echo "... src/reflection.s"
	@echo "... src/relations.o"
	@echo "... src/relations.i"
	@echo "... src/relations.s"
	@echo "... src/shared_map.o"
	@echo "... src/shared_map.i"
	@echo "... src/shared_map.s"
	@echo "... src/tests.o"
	@echo "... src/tests.i"
	@echo "... src/tests.s"
	@echo "... src/utf8.o"
	@echo "... src/utf8.i"
	@echo "... src/utf8.s"
	@echo "... src/util.o"
	@echo "... src/util.i"
	@echo "... src/util.s"
	@echo "... src/webserver.o"
	@echo "... src/webserver.i"
	@echo "... src/webserver.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

