cmake_minimum_required(VERSION 3.5)
project(netbase)

set(EMSCRIPTEN_ROOT_PATH /opt/emsdk/emscripten/1.38.12/)
set(CMAKE_C_COMPILER /opt/emsdk/emscripten/1.38.12/emcc)
set(EMSCRIPTEN_VERSION 1.38.12)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_SYSTEM_PROCESSOR x86)

#set(CMAKE_CXX_FLAGS_RELEASE "/INCREMENTAL:YES ${CMAKE_CXX_FLAGS_RELEASE}")

set (CMAKE_SKIP_RULE_DEPENDENCY TRUE) # incremental build
#set (CMAKE_SUPPRESS_REGENERATION)

# find_package( ZLIB REQUIRED )
# include_directories( ${ZLIB_INCLUDE_DIRS} )

# find_path(Readline_ROOT_DIR NAMES ../include/readline/readline.h)
# find_path(Readline_INCLUDE_DIR NAMES ../readline/readline.h HINTS ${Readline_ROOT_DIR}/include)
# find_library(Readline_LIBRARY NAMES readline HINTS ${Readline_ROOT_DIR}/lib)

mark_as_advanced(
        Readline_ROOT_DIR
        Readline_INCLUDE_DIR
        Readline_LIBRARY
)

# set(CMAKE_C_FLAGS "-Wno-write- -DNO_READLINE -Wno-logical-op-parentheses ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DNO_READLINE -DNO_ZLIB -Wno-write-strings   -Wno-logical-op-parentheses ")

set(SOURCE_FILES
        src/utf8.cpp
        src/utf8.hpp
        #        src/json.cpp
        #        src/json.h
        src/json.hpp
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
        # src/reflection.cpp
        # src/reflection.hpp
        src/relations.cpp
        src/relations.hpp
        src/shared_map.cpp
        src/shared_map.hpp
        src/tests.cpp
        src/tests.hpp
        src/util.cpp
        src/util.hpp
        src/webserver.cpp
        src/webserver.hpp src/rapid.h)

add_executable(netbase ${SOURCE_FILES})
target_link_libraries( netbase ${ZLIB_LIBRARIES} )
target_link_libraries( netbase ${Readline_LIBRARY} )
