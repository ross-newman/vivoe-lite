cmake_minimum_required(VERSION 2.8)
project(hmi-display)
message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

execute_process(COMMAND ../update.sh)
add_subdirectory(dds)
add_subdirectory(video)
add_subdirectory(hmi-display)
add_subdirectory(gva-tools)
configure_file(gva-tools/config.xml ${CMAKE_CURRENT_BINARY_DIR}/build/gva-tools/. COPYONLY)

file(COPY images/test2.png images/map.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})

include(ExternalProject)

option(HMI_ONLY "only build HMI and Tools" OFF)

if (HMI_ONLY)
# Dummy the targets our for quicker build
add_custom_target(Fast_RTPS)
add_custom_target(libosmscout)
add_custom_target(libnmea)
add_custom_target(rtp_payloader)
else (HMI_ONLY)

# ------------------------------------------------------------------------------
# Fast-RTPS
# ------------------------------------------------------------------------------
message(STATUS "SUBPROJECT_NAME = fastrtps")
set(BUILD_DOCUMENTATION OFF CACHE BOOL "Disable build documentation")

ExternalProject_Add(
    Fast_RTPS
    GIT_REPOSITORY      https://github.com/eProsima/Fast-RTPS
    GIT_TAG             v1.8.1
    GIT_SHALLOW         5
    GIT_CONFIG          fetch.recurseSubmodules=true
#    URL                 https://github.com/protocolbuffers/protobuf/releases/download/v3.8.0/protobuf-cpp-3.8.0.zip
    CMAKE_ARGS          "-DTHIRDPARTY=ON -DCMAKE_BUILD_TYPE=Release"
    PREFIX              ${CMAKE_BINARY_DIR}/external/Fast-RTPS/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/Fast-RTPS/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/Fast-RTPS/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/Fast-RTPS/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/Fast-RTPS/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/Fast-RTPS/build
    SOURCE_SUBDIR        "../src"
    INSTALL_COMMAND     ""
    TEST_COMMAND        ""
    UPDATE_DISCONNECTED 1
)

# ------------------------------------------------------------------------------
# libosmscout
# ------------------------------------------------------------------------------
message(STATUS "SUBPROJECT_NAME = libosmscout")
set(OSMSCOUT_BUILD_TEST OFF CACHE BOOL "Disable build of test library")
list(APPEND CMAKE_MODULE_PATH ../libosmscout/cmake)

ExternalProject_Add(
    libosmscout
    GIT_REPOSITORY      https://github.com/Framstag/libosmscout
#    GIT_TAG             1.1.0
    GIT_SHALLOW         1
    CMAKE_ARGS          "-DCMAKE_BUILD_TYPE=Release -DOSMSCOUT_BUILD_DOC_API=Off -DOSMSCOUT_BUILD_TESTS=Off -DOSMSCOUT_BUILD_GPX=Off -DOSMSCOUT_BUILD_MAP_AGG=Off -DOSMSCOUT_BUILD_MAP_SVG=Off -DOSMSCOUT_BUILD_MAP_OPENGL=Off -DOSMSCOUT_BUILD_MAP_QT=Off -D=OSMSCOUT_BUILD_CLIENT_QT=Off -DOSMSCOUT_BUILD_MAP_SVG=Off -DOSMSCOUT_BUILD_WEBPAGE=Off"
    PREFIX              ${CMAKE_BINARY_DIR}/external/libosmscout/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/libosmscout/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/libosmscout/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/libosmscout/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/libosmscout/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/libosmscout/build
    CONFIGURE_COMMAND   cmake ../src -DOSMSCOUT_BUILD_DOC_API=OFF -DOSMSCOUT_BUILD_TESTS=OFF -DOSMSCOUT_BUILD_TOOL_OSMSCOUT2=OFF -DOSMSCOUT_BUILD_GPX=OFF -DOSMSCOUT_BUILD_MAP_AGG=OFF -DOSMSCOUT_BUILD_MAP_SVG=OFF -DOSMSCOUT_BUILD_MAP_OPENGL=OFF -DOSMSCOUT_BUILD_MAP_QT=OFF -DOSMSCOUT_BUILD_CLIENT_QT=OFF -DOSMSCOUT_BUILD_TOOL_STYLEEDITOR=OFF -DOSMSCOUT_BUILD_MAP_SVG=OFF -DOSMSCOUT_BUILD_WEBPAGE=OFF
    BUILD_COMMAND       ${MAKE}
    SOURCE_SUBDIR       "../src"
    INSTALL_COMMAND     ""
    TEST_COMMAND        ""
    UPDATE_DISCONNECTED 1
)

# ------------------------------------------------------------------------------
# rtp-payloader
# ------------------------------------------------------------------------------
message(STATUS "SUBPROJECT_NAME = rtp-payloader")
ExternalProject_Add(
    rtp_payloader
    GIT_REPOSITORY      https://github.com/ross-newman/rtp-payloader
    GIT_SHALLOW         1
    CMAKE_ARGS          ""
    PREFIX              ${CMAKE_BINARY_DIR}/external/rtp-payloader/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/rtp-payloader/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/rtp-payloader/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/rtp-payloader/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/rtp-payloader/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/rtp-payloader/build
    INSTALL_COMMAND     ""
    UPDATE_DISCONNECTED 1
)

# ------------------------------------------------------------------------------
# libnema
# ------------------------------------------------------------------------------
message(STATUS "SUBPROJECT_NAME = libnmea")
ExternalProject_Add(
    libnmea
    GIT_REPOSITORY      https://github.com/ross-newman/nmealib
    GIT_SHALLOW         1
    CMAKE_ARGS          ""
    PREFIX              ${CMAKE_BINARY_DIR}/external/libnmea/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/libnmea/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/libnmea/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/libnmea/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/libnmea/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/libnmea/build
    INSTALL_COMMAND     ""
    UPDATE_DISCONNECTED 1
)

# ------------------------------------------------------------------------------
# Catch
# ------------------------------------------------------------------------------
message(STATUS "SUBPROJECT_NAME = catch")
list(APPEND CATCH_CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
)

ExternalProject_Add(
    catch
    GIT_REPOSITORY      https://github.com/Bareflank/catch.git
    GIT_TAG             v1.2
    GIT_SHALLOW         1
    CMAKE_ARGS          ${CATCH_CMAKE_ARGS}
    PREFIX              ${CMAKE_BINARY_DIR}/external/catch/prefix
    TMP_DIR             ${CMAKE_BINARY_DIR}/external/catch/tmp
    STAMP_DIR           ${CMAKE_BINARY_DIR}/external/catch/stamp
    DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/catch/download
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/catch/src
    BINARY_DIR          ${CMAKE_BINARY_DIR}/external/catch/build
    UPDATE_DISCONNECTED 1
)
endif (HMI_ONLY)

option(BUILD_TESTS "Build the tests" OFF)

if (BUILD_TESTS)
# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
include(CTest)
include(FindPkgConfig)

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/src/libosmscout/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/src/libosmscout-map/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/src/libosmscout-map-cairo/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout-map/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout-map-cairo/include)
include_directories(${CMAKE_BINARY_DIR}/external/rtp-payloader/src)
include_directories(${CMAKE_BINARY_DIR}/external/libnmea/src/include)
include_directories(${PROJECT_SOURCE_DIR}/../libosmscout/libosmscout/include)
include_directories(${PROJECT_SOURCE_DIR}/../libosmscout/libosmscout-map/include)
include_directories(${PROJECT_SOURCE_DIR}/../libosmscout/libosmscout-map-cairo/include)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/include)

add_library(test_catch STATIC test/test.cc)
add_dependencies(test_catch catch)

find_package(Protobuf REQUIRED)
message(STATUS "Using Protocol Buffers ${Protobuf_VERSION}...")
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROJECT_SOURCE_DIR}/hmi-display/config.proto)

add_library(business_logic STATIC ${PROTO_SRCS} hmi-display/events_gva.cc hmi-display/hmi_gva.cc hmi-display/screen_gva.cc hmi-display/renderer_gva.cc hmi-display/renderer.cc hmi-display/renderer_cairo.cc hmi-display/common/log_gva.cc hmi-display/view_gva.cc hmi-display/renderer_map.cc hmi-display/config_reader.cc )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11")
add_dependencies(business_logic libnmea hmi-video rtp_payloader libosmscout)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(SWSCALE REQUIRED libswscale)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/src/libosmscout/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/src/libosmscout-map/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/src/libosmscout-map-cairo/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout-map/include)
include_directories(${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout-map-cairo/include)
link_directories(${CMAKE_BINARY_DIR}/external/libnmea/build/src ${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout ${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout-map ${CMAKE_BINARY_DIR}/external/libosmscout/build/libosmscout-map-cairo)
target_link_libraries(business_logic ${Protobuf_LIBRARIES} ${GTK3_LIBRARIES} ${CAIRO_LIBRARIES} ${PANGO_LIBRARIES} ${SWSCALE_LIBRARIES} X11 pthread Geographic nmealib osmscout osmscout_map osmscout_map_cairo)
target_compile_definitions(business_logic PRIVATE VIRTUAL=virtual MAIN=unittest_main)
target_include_directories(business_logic PUBLIC ${GTK3_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${PANGO_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS} catch nmealib rtp-payloader video hmi-display hmi-display/common )

macro(do_test str)
    add_executable(test_${str} test/test_${str}.cc)
    add_test(test_${str} test_${str})
    target_link_libraries(test_${str}  ${Protobuf_LIBRARIES} test_catch business_logic)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/nmealib/src ${CMAKE_CURRENT_SOURCE_DIR}/build/include)
    add_dependencies(test_${str} libnmea catch)
    target_compile_definitions(test_${str} PRIVATE VIRTUAL=virtual)
endmacro(do_test)

do_test(events)
do_test(log)
#do_test(hmi)
endif (BUILD_TESTS)
