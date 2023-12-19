ExternalProject_Add(
    cimgui-external
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    GIT_TAG 1.90dock
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(cimgui-external SOURCE_DIR)

set(CIMGUI_SOURCES 
    ${SOURCE_DIR}/cimgui.cpp
    ${SOURCE_DIR}/imgui/imgui.cpp
    ${SOURCE_DIR}/imgui/imgui_draw.cpp
    ${SOURCE_DIR}/imgui/imgui_demo.cpp
    ${SOURCE_DIR}/imgui/imgui_tables.cpp
    ${SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
)

ExternalProject_Add_Step(
    cimgui-external
    fetch
    DEPENDEES download
    BYPRODUCTS ${CIMGUI_SOURCES}
)

ExternalProject_Add_Step(
    cimgui-external
    bootstrap
    COMMAND ./generator.sh -t "internal noimstrv" -c "glfw vulkan"
    DEPENDEES download
    WORKING_DIRECTORY ${SOURCE_DIR}/generator
    BYPRODUCTS 
        ${SOURCE_DIR}/generator/output/cimgui_impl.h
        ${SOURCE_DIR}/cimgui.cpp
        ${SOURCE_DIR}/cimgui.h
)

add_library(cimgui STATIC ${CIMGUI_SOURCES})
add_dependencies(cimgui cimgui-external)

target_include_directories(cimgui PUBLIC
  ${SOURCE_DIR}
  ${SOURCE_DIR}/generator/output
  ${SOURCE_DIR}/imgui
  ${SOURCE_DIR}/imgui/backends
)

target_compile_definitions(cimgui PRIVATE 
    IMGUI_IMPL_API=extern\ \"C\"
    IMGUI_DISABLE_OBSOLETE_KEYIO
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS
)