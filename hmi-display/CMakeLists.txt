include(FindPkgConfig)
project(hmi-display)
cmake_minimum_required(VERSION 2.8)
# Add project cmake modules to path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# We probably don't want this to run on every build.
option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
    
    set(COVERAGE_SRCS main.cpp rendererSdl.cpp rendererCairo.cpp rendererGva.cpp screenGva.cpp eventsGva.cpp ./common/debug.cpp ./common/logGva.cpp hmiGva.cpp viewGva.cpp)

    # Create the coveralls target.
    coveralls_setup(
        "${COVERAGE_SRCS}" # The source files.
        ON                 # If we should upload.
        "${PROJECT_SOURCE_DIR}/CMakeModules/") # (Optional) Alternate project cmake module path.
endif()

include_directories(../rtp-payloader)
include_directories(../video)
include_directories(../nmealib/include)
include_directories(./rtp-payloader)
include_directories(./video)
add_executable(hmi-display main.cpp rendererSdl.cpp rendererCairo.cpp rendererGva.cpp screenGva.cpp eventsGva.cpp ./common/debug.cpp ./common/logGva.cpp hmiGva.cpp viewGva.cpp)
#add_subdirectory(dds)
#add_subdirectory(display)
include_directories ()

add_subdirectory(common)
#set (CMAKE_CXX_FLAGS " -std=c++14 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -pedantic-errors ")
set (CMAKE_CXX_FLAGS "-w -std=c++11 --coverage")
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(SWSCALE REQUIRED libswscale)
target_link_libraries(hmi-display SDL2 X11 png pthread hmi-video rtp-payloader nmealib)
target_link_libraries(hmi-display ${CAIRO_LIBRARIES} ${SWSCALE_LIBRARIES})
target_include_directories(hmi-display PUBLIC ${CAIRO_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS} common dds display)
target_compile_options(hmi-display PUBLIC ${CAIRO_CFLAGS_OTHER})
install(TARGETS hmi-display DESTINATION bin)
