cmake_minimum_required(VERSION 3.0.0)

project(poulpe)

add_subdirectory(externals/sake)

set(POULPE_SOURCES
    src/poulpe/display.c
    src/poulpe/editor.c
    src/poulpe/error.c
    src/poulpe/log.c
)

add_library(poulpe ${POULPE_SOURCES})

target_link_libraries(poulpe
    sake
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(poulpe PRIVATE
    inc)

set_property(TARGET poulpe PROPERTY C_STANDARD 17)

target_compile_options(poulpe PRIVATE 
    -Wall -Wextra -Wpedantic)

add_subdirectory(demo)