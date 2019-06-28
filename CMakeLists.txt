cmake_minimum_required(VERSION 2.8)
project(hmi-display)

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

execute_process(COMMAND git submodule update --init) 
execute_process(COMMAND ../update.sh) 
add_subdirectory(Fast-RTPS)
add_subdirectory(hmi-display)
add_subdirectory(gva-tools)
configure_file(gva-tools/config.xml ${CMAKE_CURRENT_BINARY_DIR}/build/gva-tools/. COPYONLY)
add_subdirectory(rtp-payloader)
add_subdirectory(video)
add_subdirectory(nmealib)
add_subdirectory(dds)
option(THIRDPARTY "Turn no third party modules" ON)
set(THIRDPARTY ON CACHE BOOL "enable X functionality")
#add_subdirectory(libosmscout)
message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
file(COPY images/test2.png images/map.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})

# ------------------------------------------------------------------------------
# Catch
# ------------------------------------------------------------------------------
include(ExternalProject)

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
)

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
include(CTest)
include(FindPkgConfig)

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

include_directories(include)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/include)

add_library(test_catch STATIC test/test.cpp)
add_dependencies(test_catch catch)
add_library(business_logic STATIC hmi-display/eventsGva.cpp hmi-display/hmiGva.cpp hmi-display/screenGva.cpp hmi-display/rendererGva.cpp  hmi-display/rendererCairo.cpp  hmi-display/common/logGva.cpp hmi-display/viewGva.cpp )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11")
add_dependencies(business_logic nmealib hmi-video rtp-payloader)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(SWSCALE REQUIRED libswscale)
target_link_libraries(business_logic ${CAIRO_LIBRARIES} ${SWSCALE_LIBRARIES} X11 pthread Geographic nmealib)
target_compile_definitions(business_logic PRIVATE VIRTUAL=virtual MAIN=unittest_main)
target_include_directories(business_logic PUBLIC ${CAIRO_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS} catch nmealib/include rtp-payloader video hmi-display hmi-display/common )

macro(do_test str)
    add_executable(test_${str} test/test_${str}.cpp)
    add_test(test_${str} test_${str})
    target_link_libraries(test_${str} test_catch)
    target_link_libraries(test_${str} business_logic)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/nmealib/src ${CMAKE_CURRENT_SOURCE_DIR}/build/include)
    add_dependencies(test_${str} nmealib)
    add_dependencies(test_${str} catch)
    target_compile_definitions(test_${str} PRIVATE VIRTUAL=virtual)
endmacro(do_test)

do_test(events)
do_test(log)
#do_test(hmi)
