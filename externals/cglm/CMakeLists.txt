ExternalProject_Add(
    cglm-external
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG v0.9.1
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(cglm-external SOURCE_DIR BINARY_DIR)

add_library(cglm INTERFACE)
add_dependencies(cglm cglm-external)

target_include_directories(cglm SYSTEM INTERFACE ${SOURCE_DIR}/include)
target_link_libraries(cglm INTERFACE ${BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}cglm${CMAKE_SHARED_LIBRARY_SUFFIX})