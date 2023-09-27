include(FetchContent)

FetchContent_Declare(
    sake
    GIT_REPOSITORY https://github.com/wervin/sake.git
    GIT_TAG v0.0.2
)

FetchContent_MakeAvailable(sake)