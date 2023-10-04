include(FetchContent)

FetchContent_Declare(
    cimgui
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    GIT_TAG 1.89.8dock
)

FetchContent_GetProperties(cimgui)

if(NOT ${cimgui_POPULATED})
    FetchContent_Populate(cimgui)
    message("remove default cimgui_impl.h")
    file(REMOVE ${cimgui_SOURCE_DIR}/generator/output/cimgui_impl.h)
endif()

add_custom_command(
    OUTPUT ${cimgui_SOURCE_DIR}/generator/output/cimgui_impl.h
    COMMAND luajit ./generator.lua gcc "internal" glfw vulkan
    WORKING_DIRECTORY ${cimgui_SOURCE_DIR}/generator
)

add_custom_target(cimgui_backend
    DEPENDS ${cimgui_SOURCE_DIR}/generator/output/cimgui_impl.h
)

add_library(cimgui_core STATIC
    ${cimgui_SOURCE_DIR}/cimgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
)

add_dependencies(cimgui_core 
    cimgui_backend
)

target_include_directories(cimgui_core PUBLIC
  ${cimgui_SOURCE_DIR}
  ${cimgui_SOURCE_DIR}/generator/output
  ${cimgui_SOURCE_DIR}/imgui
  ${cimgui_SOURCE_DIR}/imgui/backends
)

target_compile_definitions(cimgui_core PRIVATE 
    IMGUI_IMPL_API=extern\ \"C\"
)