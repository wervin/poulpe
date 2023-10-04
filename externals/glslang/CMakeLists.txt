include(FetchContent)

FetchContent_Declare(
    glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG sdk-1.3.261.0
)

FetchContent_MakeAvailable(glslang)