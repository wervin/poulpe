ExternalProject_Add(
    tree-sitter-glsl-external
    GIT_REPOSITORY https://github.com/theHamsta/tree-sitter-glsl.git
    GIT_TAG v0.1.6
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(tree-sitter-glsl-external SOURCE_DIR)

set(SOURCES 
    ${SOURCE_DIR}/src/parser.c
)

ExternalProject_Add_Step(
    tree-sitter-glsl-external
    fetch
    DEPENDEES download
    BYPRODUCTS ${SOURCES}
)

add_library(tree-sitter-glsl STATIC ${SOURCES})
add_dependencies(tree-sitter-glsl tree-sitter-glsl-external)
target_include_directories(tree-sitter-glsl PRIVATE ${SOURCE_DIR}/src)