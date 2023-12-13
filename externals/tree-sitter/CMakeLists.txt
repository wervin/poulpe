ExternalProject_Add(
    tree-sitter-external
    GIT_REPOSITORY  https://github.com/tree-sitter/tree-sitter.git
    GIT_TAG v0.20.8
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    BUILD_IN_SOURCE true
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(tree-sitter-external SOURCE_DIR)

add_library(tree-sitter INTERFACE)
add_dependencies(tree-sitter tree-sitter-external)

target_include_directories(tree-sitter INTERFACE ${SOURCE_DIR}/lib/include)
target_link_libraries(tree-sitter INTERFACE ${SOURCE_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}tree-sitter${CMAKE_STATIC_LIBRARY_SUFFIX})