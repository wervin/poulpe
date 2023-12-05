ExternalProject_Add(
    sake-external
    GIT_REPOSITORY https://github.com/wervin/sake.git
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(sake-external SOURCE_DIR BINARY_DIR)

add_library(sake INTERFACE)
add_dependencies(sake sake-external)

target_include_directories(sake INTERFACE ${SOURCE_DIR}/inc)
target_link_libraries(sake INTERFACE ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}sake${CMAKE_STATIC_LIBRARY_SUFFIX})
