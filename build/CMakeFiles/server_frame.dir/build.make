# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /mnt/c/Users/Administrator/ubuntu/server_frame

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/Administrator/ubuntu/server_frame/build

# Include any dependencies generated for this target.
include CMakeFiles/server_frame.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/server_frame.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server_frame.dir/flags.make

CMakeFiles/server_frame.dir/server_frame/log.cc.o: CMakeFiles/server_frame.dir/flags.make
CMakeFiles/server_frame.dir/server_frame/log.cc.o: ../server_frame/log.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/server_frame.dir/server_frame/log.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"server_frame/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server_frame.dir/server_frame/log.cc.o -c /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/log.cc

CMakeFiles/server_frame.dir/server_frame/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_frame.dir/server_frame/log.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/log.cc > CMakeFiles/server_frame.dir/server_frame/log.cc.i

CMakeFiles/server_frame.dir/server_frame/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_frame.dir/server_frame/log.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/log.cc -o CMakeFiles/server_frame.dir/server_frame/log.cc.s

CMakeFiles/server_frame.dir/server_frame/log.cc.o.requires:

.PHONY : CMakeFiles/server_frame.dir/server_frame/log.cc.o.requires

CMakeFiles/server_frame.dir/server_frame/log.cc.o.provides: CMakeFiles/server_frame.dir/server_frame/log.cc.o.requires
	$(MAKE) -f CMakeFiles/server_frame.dir/build.make CMakeFiles/server_frame.dir/server_frame/log.cc.o.provides.build
.PHONY : CMakeFiles/server_frame.dir/server_frame/log.cc.o.provides

CMakeFiles/server_frame.dir/server_frame/log.cc.o.provides.build: CMakeFiles/server_frame.dir/server_frame/log.cc.o


CMakeFiles/server_frame.dir/server_frame/util.cc.o: CMakeFiles/server_frame.dir/flags.make
CMakeFiles/server_frame.dir/server_frame/util.cc.o: ../server_frame/util.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/server_frame.dir/server_frame/util.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"server_frame/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server_frame.dir/server_frame/util.cc.o -c /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/util.cc

CMakeFiles/server_frame.dir/server_frame/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_frame.dir/server_frame/util.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/util.cc > CMakeFiles/server_frame.dir/server_frame/util.cc.i

CMakeFiles/server_frame.dir/server_frame/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_frame.dir/server_frame/util.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/util.cc -o CMakeFiles/server_frame.dir/server_frame/util.cc.s

CMakeFiles/server_frame.dir/server_frame/util.cc.o.requires:

.PHONY : CMakeFiles/server_frame.dir/server_frame/util.cc.o.requires

CMakeFiles/server_frame.dir/server_frame/util.cc.o.provides: CMakeFiles/server_frame.dir/server_frame/util.cc.o.requires
	$(MAKE) -f CMakeFiles/server_frame.dir/build.make CMakeFiles/server_frame.dir/server_frame/util.cc.o.provides.build
.PHONY : CMakeFiles/server_frame.dir/server_frame/util.cc.o.provides

CMakeFiles/server_frame.dir/server_frame/util.cc.o.provides.build: CMakeFiles/server_frame.dir/server_frame/util.cc.o


CMakeFiles/server_frame.dir/server_frame/config.cc.o: CMakeFiles/server_frame.dir/flags.make
CMakeFiles/server_frame.dir/server_frame/config.cc.o: ../server_frame/config.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/server_frame.dir/server_frame/config.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"server_frame/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server_frame.dir/server_frame/config.cc.o -c /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/config.cc

CMakeFiles/server_frame.dir/server_frame/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_frame.dir/server_frame/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/config.cc > CMakeFiles/server_frame.dir/server_frame/config.cc.i

CMakeFiles/server_frame.dir/server_frame/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_frame.dir/server_frame/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/config.cc -o CMakeFiles/server_frame.dir/server_frame/config.cc.s

CMakeFiles/server_frame.dir/server_frame/config.cc.o.requires:

.PHONY : CMakeFiles/server_frame.dir/server_frame/config.cc.o.requires

CMakeFiles/server_frame.dir/server_frame/config.cc.o.provides: CMakeFiles/server_frame.dir/server_frame/config.cc.o.requires
	$(MAKE) -f CMakeFiles/server_frame.dir/build.make CMakeFiles/server_frame.dir/server_frame/config.cc.o.provides.build
.PHONY : CMakeFiles/server_frame.dir/server_frame/config.cc.o.provides

CMakeFiles/server_frame.dir/server_frame/config.cc.o.provides.build: CMakeFiles/server_frame.dir/server_frame/config.cc.o


CMakeFiles/server_frame.dir/server_frame/thread.cc.o: CMakeFiles/server_frame.dir/flags.make
CMakeFiles/server_frame.dir/server_frame/thread.cc.o: ../server_frame/thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/server_frame.dir/server_frame/thread.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"server_frame/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server_frame.dir/server_frame/thread.cc.o -c /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/thread.cc

CMakeFiles/server_frame.dir/server_frame/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_frame.dir/server_frame/thread.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/thread.cc > CMakeFiles/server_frame.dir/server_frame/thread.cc.i

CMakeFiles/server_frame.dir/server_frame/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_frame.dir/server_frame/thread.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/thread.cc -o CMakeFiles/server_frame.dir/server_frame/thread.cc.s

CMakeFiles/server_frame.dir/server_frame/thread.cc.o.requires:

.PHONY : CMakeFiles/server_frame.dir/server_frame/thread.cc.o.requires

CMakeFiles/server_frame.dir/server_frame/thread.cc.o.provides: CMakeFiles/server_frame.dir/server_frame/thread.cc.o.requires
	$(MAKE) -f CMakeFiles/server_frame.dir/build.make CMakeFiles/server_frame.dir/server_frame/thread.cc.o.provides.build
.PHONY : CMakeFiles/server_frame.dir/server_frame/thread.cc.o.provides

CMakeFiles/server_frame.dir/server_frame/thread.cc.o.provides.build: CMakeFiles/server_frame.dir/server_frame/thread.cc.o


CMakeFiles/server_frame.dir/server_frame/mutex.cc.o: CMakeFiles/server_frame.dir/flags.make
CMakeFiles/server_frame.dir/server_frame/mutex.cc.o: ../server_frame/mutex.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/server_frame.dir/server_frame/mutex.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"server_frame/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server_frame.dir/server_frame/mutex.cc.o -c /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/mutex.cc

CMakeFiles/server_frame.dir/server_frame/mutex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_frame.dir/server_frame/mutex.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/mutex.cc > CMakeFiles/server_frame.dir/server_frame/mutex.cc.i

CMakeFiles/server_frame.dir/server_frame/mutex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_frame.dir/server_frame/mutex.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/mutex.cc -o CMakeFiles/server_frame.dir/server_frame/mutex.cc.s

CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.requires:

.PHONY : CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.requires

CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.provides: CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.requires
	$(MAKE) -f CMakeFiles/server_frame.dir/build.make CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.provides.build
.PHONY : CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.provides

CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.provides.build: CMakeFiles/server_frame.dir/server_frame/mutex.cc.o


CMakeFiles/server_frame.dir/server_frame/fiber.cc.o: CMakeFiles/server_frame.dir/flags.make
CMakeFiles/server_frame.dir/server_frame/fiber.cc.o: ../server_frame/fiber.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/server_frame.dir/server_frame/fiber.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"server_frame/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server_frame.dir/server_frame/fiber.cc.o -c /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/fiber.cc

CMakeFiles/server_frame.dir/server_frame/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_frame.dir/server_frame/fiber.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/fiber.cc > CMakeFiles/server_frame.dir/server_frame/fiber.cc.i

CMakeFiles/server_frame.dir/server_frame/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_frame.dir/server_frame/fiber.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"server_frame/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/fiber.cc -o CMakeFiles/server_frame.dir/server_frame/fiber.cc.s

CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.requires:

.PHONY : CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.requires

CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.provides: CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.requires
	$(MAKE) -f CMakeFiles/server_frame.dir/build.make CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.provides.build
.PHONY : CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.provides

CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.provides.build: CMakeFiles/server_frame.dir/server_frame/fiber.cc.o


# Object files for target server_frame
server_frame_OBJECTS = \
"CMakeFiles/server_frame.dir/server_frame/log.cc.o" \
"CMakeFiles/server_frame.dir/server_frame/util.cc.o" \
"CMakeFiles/server_frame.dir/server_frame/config.cc.o" \
"CMakeFiles/server_frame.dir/server_frame/thread.cc.o" \
"CMakeFiles/server_frame.dir/server_frame/mutex.cc.o" \
"CMakeFiles/server_frame.dir/server_frame/fiber.cc.o"

# External object files for target server_frame
server_frame_EXTERNAL_OBJECTS =

../lib/libserver_frame.so: CMakeFiles/server_frame.dir/server_frame/log.cc.o
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/server_frame/util.cc.o
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/server_frame/config.cc.o
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/server_frame/thread.cc.o
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/server_frame/mutex.cc.o
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/server_frame/fiber.cc.o
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/build.make
../lib/libserver_frame.so: CMakeFiles/server_frame.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX shared library ../lib/libserver_frame.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server_frame.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server_frame.dir/build: ../lib/libserver_frame.so

.PHONY : CMakeFiles/server_frame.dir/build

CMakeFiles/server_frame.dir/requires: CMakeFiles/server_frame.dir/server_frame/log.cc.o.requires
CMakeFiles/server_frame.dir/requires: CMakeFiles/server_frame.dir/server_frame/util.cc.o.requires
CMakeFiles/server_frame.dir/requires: CMakeFiles/server_frame.dir/server_frame/config.cc.o.requires
CMakeFiles/server_frame.dir/requires: CMakeFiles/server_frame.dir/server_frame/thread.cc.o.requires
CMakeFiles/server_frame.dir/requires: CMakeFiles/server_frame.dir/server_frame/mutex.cc.o.requires
CMakeFiles/server_frame.dir/requires: CMakeFiles/server_frame.dir/server_frame/fiber.cc.o.requires

.PHONY : CMakeFiles/server_frame.dir/requires

CMakeFiles/server_frame.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server_frame.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server_frame.dir/clean

CMakeFiles/server_frame.dir/depend:
	cd /mnt/c/Users/Administrator/ubuntu/server_frame/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/Administrator/ubuntu/server_frame /mnt/c/Users/Administrator/ubuntu/server_frame /mnt/c/Users/Administrator/ubuntu/server_frame/build /mnt/c/Users/Administrator/ubuntu/server_frame/build /mnt/c/Users/Administrator/ubuntu/server_frame/build/CMakeFiles/server_frame.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server_frame.dir/depend
