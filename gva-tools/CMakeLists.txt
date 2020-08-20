project(gva-tool)
cmake_minimum_required(VERSION 3.0)

find_package(LibXml2 REQUIRED)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}/external/)
include_directories(${CMAKE_SOURCE_DIR}/hmi-display/)
include_directories(${CMAKE_SOURCE_DIR}/hmi-display/common)
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})
add_executable(gva-tool gva-tool.cc gva-display.cc)
add_dependencies(gva-tool Fast_RTPS)
set (CMAKE_CXX_FLAGS "-Wwrite-strings -w")
target_link_libraries(gva-tool ncurses xml2  ${GTK3_LIBRARIES})
