ExternalProject_Add(
    tree-sitter-json-external
    GIT_REPOSITORY https://github.com/tree-sitter/tree-sitter-json.git
    GIT_TAG v0.20.1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(tree-sitter-json-external SOURCE_DIR)

set(SOURCES 
    ${SOURCE_DIR}/src/parser.c
)

ExternalProject_Add_Step(
    tree-sitter-json-external
    fetch
    DEPENDEES download
    BYPRODUCTS ${SOURCES}
)

add_library(tree-sitter-json STATIC ${SOURCES})
add_dependencies(tree-sitter-json tree-sitter-json-external)
target_include_directories(tree-sitter-json PRIVATE ${SOURCE_DIR}/src)