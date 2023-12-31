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
    sake
    cimgui
    poulpe
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(demo PRIVATE
    inc)

set_property(TARGET demo PROPERTY C_STANDARD 17)

target_compile_options(demo PRIVATE 
    -Wall -Wextra -Wpedantic)

target_compile_definitions(demo PRIVATE 
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    GLFW_INCLUDE_VULKAN
    CIMGUI_USE_GLFW
    CIMGUI_USE_VULKAN
)

add_custom_command(
    TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/demo/assets
            ${CMAKE_BINARY_DIR}/demo/assets
)

add_custom_command(
    TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/demo/tests
            ${CMAKE_BINARY_DIR}/demo/tests
)           