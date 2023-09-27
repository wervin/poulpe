set(POULPE_EXAMPLE_SOURCES
    main.c
)

add_executable(poulpe-example ${POULPE_EXAMPLE_SOURCES})

target_link_libraries(poulpe-example
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

set_property(TARGET poulpe PROPERTY C_STANDARD 17)

target_compile_options(poulpe PRIVATE 
    -Wall -Wextra -Wpedantic)