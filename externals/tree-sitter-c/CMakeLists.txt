ExternalProject_Add(
    tree-sitter-c-external
    GIT_REPOSITORY https://github.com/tree-sitter/tree-sitter-c.git
    GIT_TAG v0.20.6
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(tree-sitter-c-external SOURCE_DIR)

set(SOURCES 
    ${SOURCE_DIR}/src/parser.c
)

ExternalProject_Add_Step(
    tree-sitter-c-external
    fetch
    DEPENDEES download
    BYPRODUCTS ${SOURCES}
)

add_library(tree-sitter-c STATIC ${SOURCES})
add_dependencies(tree-sitter-c tree-sitter-c-external)
target_include_directories(tree-sitter-c PRIVATE ${SOURCE_DIR}/src)