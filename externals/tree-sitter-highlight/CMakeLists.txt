FetchContent_Declare(
    tree-sitter
    GIT_REPOSITORY  https://github.com/tree-sitter/tree-sitter.git
    GIT_TAG v0.20.8
)

FetchContent_GetProperties(tree-sitter)
if(NOT ${tree-sitter_POPULATED})
    FetchContent_Populate(tree-sitter)
endif()