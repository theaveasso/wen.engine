project(ShaderCross)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross
    GIT_TAG main
)
FetchContent_MakeAvailable(SDL3)

