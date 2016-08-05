# Copyright (c) 2016 Fabian Schuiki
cmake_minimum_required(VERSION 2.6)
project(libgds)

# Default to a release build, making the project easier to package. If you plan
# on writing code, call cmake with the -DCMAKE_BUILD_TYPE=debug option.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE release)
endif()
message("## libgds ${CMAKE_BUILD_TYPE} build")

# Compiler Flags
include_directories(${PROJECT_BINARY_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Werror-implicit-function-declaration -Wno-unused-parameter -Wno-unused-label")

configure_file(include/gds.h gds.h)

add_library(gds STATIC
	src/lib.c
	src/elem.c
	src/reader.c
	src/real.c
	src/struct.c
	src/util-ptrset.c
	src/util.c
	src/writer.c
	src/parser.c
)

# include_directories(/usr/include/cairo)
add_executable(debug src/debug.c)
target_link_libraries(debug gds m)
# target_link_libraries(debug gds cairo)
