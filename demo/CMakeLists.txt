set(DEMO_SOURCES
    src/demo/application.c
    src/demo/error.c
    src/demo/log.c
    src/demo/main.c
    src/demo/renderer.c
    src/demo/ui.c
    src/demo/window.c)

add_executable(demo ${DEMO_SOURCES})

target_link_libraries(demo
    m
    cglm
    glslang
    sake
    cimgui_core
    poulpe
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(demo PRIVATE
    inc)

set_property(TARGET demo PROPERTY C_STANDARD 17)

target_compile_options(demo PRIVATE 
    -Wall -Wextra -Wpedantic)