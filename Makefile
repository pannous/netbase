# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /opt/clion-2017.2.3/bin/cmake/bin/cmake

# The command to remove a file.
RM = /opt/clion-2017.2.3/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/me/netbase

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/me/netbase

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/opt/clion-2017.2.3/bin/cmake/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/opt/clion-2017.2.3/bin/cmake/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/me/netbase/CMakeFiles /home/me/netbase/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/me/netbase/CMakeFiles 0
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

# target to build an object file
src/console.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/console.o
.PHONY : src/console.o

# target to preprocess a source file
src/console.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/console.i
.PHONY : src/console.i

# target to generate assembly for a file
src/console.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/console.s
.PHONY : src/console.s

# target to build an object file
src/export.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/export.o
.PHONY : src/export.o

# target to preprocess a source file
src/export.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/export.i
.PHONY : src/export.i

# target to generate assembly for a file
src/export.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/export.s
.PHONY : src/export.s

# target to build an object file
src/import.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/import.o
.PHONY : src/import.o

# target to preprocess a source file
src/import.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/import.i
.PHONY : src/import.i

# target to generate assembly for a file
src/import.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/import.s
.PHONY : src/import.s

# target to build an object file
src/init.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/init.o
.PHONY : src/init.o

# target to preprocess a source file
src/init.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/init.i
.PHONY : src/init.i

# target to generate assembly for a file
src/init.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/init.s
.PHONY : src/init.s

# target to build an object file
src/md5.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/md5.o
.PHONY : src/md5.o

# target to preprocess a source file
src/md5.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/md5.i
.PHONY : src/md5.i

# target to generate assembly for a file
src/md5.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/md5.s
.PHONY : src/md5.s

# target to build an object file
src/netbase.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/netbase.o
.PHONY : src/netbase.o

# target to preprocess a source file
src/netbase.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/netbase.i
.PHONY : src/netbase.i

# target to generate assembly for a file
src/netbase.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/netbase.s
.PHONY : src/netbase.s

# target to build an object file
src/query.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/query.o
.PHONY : src/query.o

# target to preprocess a source file
src/query.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/query.i
.PHONY : src/query.i

# target to generate assembly for a file
src/query.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/query.s
.PHONY : src/query.s

# target to build an object file
src/reflection.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/reflection.o
.PHONY : src/reflection.o

# target to preprocess a source file
src/reflection.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/reflection.i
.PHONY : src/reflection.i

# target to generate assembly for a file
src/reflection.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/reflection.s
.PHONY : src/reflection.s

# target to build an object file
src/relations.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/relations.o
.PHONY : src/relations.o

# target to preprocess a source file
src/relations.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/relations.i
.PHONY : src/relations.i

# target to generate assembly for a file
src/relations.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/relations.s
.PHONY : src/relations.s

# target to build an object file
src/shared_map.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/shared_map.o
.PHONY : src/shared_map.o

# target to preprocess a source file
src/shared_map.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/shared_map.i
.PHONY : src/shared_map.i

# target to generate assembly for a file
src/shared_map.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/shared_map.s
.PHONY : src/shared_map.s

# target to build an object file
src/tests.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/tests.o
.PHONY : src/tests.o

# target to preprocess a source file
src/tests.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/tests.i
.PHONY : src/tests.i

# target to generate assembly for a file
src/tests.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/tests.s
.PHONY : src/tests.s

# target to build an object file
src/utf8.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.o
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.o
.PHONY : src/utf8.o

# target to preprocess a source file
src/utf8.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.i
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.i
.PHONY : src/utf8.i

# target to generate assembly for a file
src/utf8.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.s
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/utf8.s
.PHONY : src/utf8.s

# target to build an object file
src/util.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/util.o
.PHONY : src/util.o

# target to preprocess a source file
src/util.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/util.i
.PHONY : src/util.i

# target to generate assembly for a file
src/util.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/util.s
.PHONY : src/util.s

# target to build an object file
src/webserver.o:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/webserver.o
.PHONY : src/webserver.o

# target to preprocess a source file
src/webserver.i:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/webserver.i
.PHONY : src/webserver.i

# target to generate assembly for a file
src/webserver.s:
	$(MAKE) -f CMakeFiles/netbase.dir/build.make CMakeFiles/netbase.dir/src/webserver.s
.PHONY : src/webserver.s

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

