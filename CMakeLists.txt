cmake_minimum_required(VERSION 3.10)

project(poulpe)

include(ExternalProject)

add_subdirectory(externals/cglm)
add_subdirectory(externals/cimgui)
add_subdirectory(externals/sake)
add_subdirectory(externals/tree-sitter)
add_subdirectory(externals/tree-sitter-c)
add_subdirectory(externals/tree-sitter-glsl)
add_subdirectory(externals/tree-sitter-json)

set(POULPE_SOURCES
    src/poulpe/components/cursor.c
    src/poulpe/components/cursorinfo.c
    src/poulpe/components/eofinfo.c
    src/poulpe/components/indentinfo.c
    src/poulpe/components/linenumber.c
    src/poulpe/components/selection.c
    src/poulpe/components/statusbar.c
    src/poulpe/components/textedit.c
    src/poulpe/components/textinfo.c
    src/poulpe/components/textview.c
    src/poulpe/component.c
    src/poulpe/editor.c
    src/poulpe/error.c
    src/poulpe/io.c
    src/poulpe/language.c
    src/poulpe/log.c
    src/poulpe/text.c
    src/poulpe/textbuffer.c
    src/poulpe/theme.c
)

add_library(poulpe ${POULPE_SOURCES})

target_link_libraries(poulpe
    m
    sake
    cglm
    cimgui
    tree-sitter
    tree-sitter-c
    tree-sitter-glsl
    tree-sitter-json
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(poulpe PUBLIC
    inc)

set_property(TARGET poulpe PROPERTY C_STANDARD 17)

target_compile_options(poulpe PRIVATE 
    -Wall -Wextra -Wpedantic)

target_compile_definitions(poulpe PRIVATE 
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    GLFW_INCLUDE_VULKAN
    CIMGUI_USE_GLFW
    CIMGUI_USE_VULKAN
)

add_subdirectory(demo)