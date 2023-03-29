cmake_minimum_required(VERSION 3.5)
project(netbase)

set(DEBUG 1)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE on)

#set(CMAKE_CXX_FLAGS_RELEASE "/INCREMENTAL:YES ${CMAKE_CXX_FLAGS_RELEASE}")

set (CMAKE_SKIP_RULE_DEPENDENCY TRUE) # incremental build
#set (CMAKE_SUPPRESS_REGENERATION)

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )

# Add -O0 to remove optimizations when using gcc
IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

find_path(Readline_ROOT_DIR NAMES include/readline/readline.h)
find_path(Readline_INCLUDE_DIR NAMES readline/readline.h HINTS ${Readline_ROOT_DIR}/include)
find_library(Readline_LIBRARY NAMES readline HINTS ${Readline_ROOT_DIR}/lib)

if(Readline_INCLUDE_DIR AND Readline_LIBRARY AND Ncurses_LIBRARY)
    set(READLINE_FOUND TRUE)
else(Readline_INCLUDE_DIR AND Readline_LIBRARY AND Ncurses_LIBRARY)
    FIND_LIBRARY(Readline_LIBRARY NAMES readline)
    include(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(Readline DEFAULT_MSG Readline_INCLUDE_DIR Readline_LIBRARY )
    MARK_AS_ADVANCED(Readline_INCLUDE_DIR Readline_LIBRARY)
endif(Readline_INCLUDE_DIR AND Readline_LIBRARY AND Ncurses_LIBRARY)

mark_as_advanced(
        Readline_ROOT_DIR
        Readline_INCLUDE_DIR
        Readline_LIBRARY
)
include_directories( ${Readline_INCLUDE_DIR} )

#find_package( Readline)
#find_package( READLINE )

#-std=c99 -Wall -j8
set(CMAKE_C_FLAGS "-lreadline -lz -Wno-write-strings ")
set(CMAKE_CXX_FLAGS "-lreadline -lz -Wno-write-strings  ${CMAKE_CXX_FLAGS}")

if(ECHSE)
 set(CMAKE_CXX_FLAGS "-DECHSE ${CMAKE_CXX_FLAGS}")
endif()

if(DEBUG)
    set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")
endif()


set(SOURCE_FILES
        src/utf8.cpp
        src/utf8.hpp
        #        src/json.cpp
        #        src/json.h
#        src/json.hpp
        src/md5.cpp
        src/md5.h
        src/console.cpp
        src/console.hpp
        src/export.cpp
        src/export.hpp
        src/import.cpp
        src/import.hpp
        src/init.cpp
        src/init.hpp
        src/netbase.cpp
        src/netbase.hpp
        src/query.cpp
        src/query.hpp
        src/reflection.cxx
        src/reflection.hpp
        src/relations.cpp
        src/relations.hpp
#        src/shared_map.cpp
#        src/shared_map.hpp
        src/tests.cpp
        src/tests.hpp
        src/util.cpp
        src/util.hpp
        src/webserver.cpp
        src/webserver.hpp src/rapid.h)

add_executable(netbase ${SOURCE_FILES})
target_link_libraries( netbase ${ZLIB_LIBRARIES} )
#target_link_libraries( netbase ${Readline_LIBRARY} )
TARGET_LINK_LIBRARIES( netbase readline ) # => libreadline