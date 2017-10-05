cmake_minimum_required(VERSION 3.8)
project(netbase)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS_RELEASE "/INCREMENTAL:YES ${CMAKE_CXX_FLAGS_RELEASE}")

set (CMAKE_SKIP_RULE_DEPENDENCY TRUE) # incremental build
#set (CMAKE_SUPPRESS_REGENERATION)

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
#find_package( READLINE )

#-std=c99 -Wall -j8
set(CMAKE_C_FLAGS "-lreadline -lz -Wno-write-strings ")
set(CMAKE_CXX_FLAGS "-lreadline -lz -Wno-write-strings  ${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
        src/console.cpp
        src/console.hpp
        src/export.cpp
        src/export.hpp
        src/import.cpp
        src/import.hpp
        src/init.cpp
        src/init.hpp
        src/md5.cpp
        src/md5.h
        src/netbase.cpp
        src/netbase.hpp
        src/query.cpp
        src/query.hpp
        src/reflection.cpp
        src/reflection.hpp
        src/relations.cpp
        src/relations.hpp
        src/shared_map.cpp
        src/shared_map.hpp
        src/tests.cpp
        src/tests.hpp
        src/utf8.c
        src/utf8.cpp
        src/utf8.h
        src/utf8.hpp
        src/util.cpp
        src/util.hpp
        src/webserver.cpp
        src/webserver.hpp)

add_executable(netbase ${SOURCE_FILES})
target_link_libraries( netbase ${ZLIB_LIBRARIES} )
