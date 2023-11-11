cmake_minimum_required(VERSION 3.0.0)

project(poulpe)

add_subdirectory(externals/cglm)
add_subdirectory(externals/cimgui)
add_subdirectory(externals/glslang)
add_subdirectory(externals/sake)

set(POULPE_SOURCES
    src/poulpe/components/layout.c
    src/poulpe/components/rectangle.c
    src/poulpe/components/tabitem.c
    src/poulpe/components/tabmenu.c
    src/poulpe/components/tabview.c
    src/poulpe/component.c
    src/poulpe/editor.c
    src/poulpe/error.c
    src/poulpe/log.c
    src/poulpe/theme.c
)

add_library(poulpe ${POULPE_SOURCES})

target_link_libraries(poulpe
    m
    cglm
    glslang
    sake
    cimgui_core
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(poulpe PUBLIC
    inc)

set_property(TARGET poulpe PROPERTY C_STANDARD 17)

target_compile_options(poulpe PRIVATE 
    -Wall -Wextra -Wpedantic)

add_subdirectory(demo)