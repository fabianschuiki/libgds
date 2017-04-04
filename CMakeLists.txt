# Copyright (c) 2016 Fabian Schuiki
cmake_minimum_required(VERSION 2.6)
project(libgds)

# Default to a release build, making the project easier to package. If you plan
# on writing code, call cmake with the -DCMAKE_BUILD_TYPE=debug option.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE release)
endif()
message("## libgds ${CMAKE_BUILD_TYPE} build")


# Versioning
set(GDS_VERSION_MAJOR 0)
set(GDS_VERSION_MINOR 1)
set(GDS_VERSION_PATCH 0)
set(GDS_VERSION "${GDS_VERSION_MAJOR}.${GDS_VERSION_MINOR}.${GDS_VERSION_PATCH}")
message("## libgds version ${GDS_VERSION}")


# Compiler Flags
include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Werror-implicit-function-declaration -Wno-unused-parameter -Wno-unused-label -fPIC")


# Load the header prefix and suffix. These contain common parts of the header
# files and shall be used by the individual libraries to assemble their proper
# header.
file(READ include/prefix.h HEADER_PREFIX)
file(READ include/suffix.h HEADER_SUFFIX)


# Build the library.
configure_file(include/gds.h include/gds.h)
add_library(gds
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


# Build the debug program.
add_executable(gdsdebug
	src/debug.c
)
target_link_libraries(gdsdebug gds)


# Configure the installation.
install(TARGETS gds gdsdebug ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(FILES ${PROJECT_BINARY_DIR}/include/gds.h DESTINATION include)
