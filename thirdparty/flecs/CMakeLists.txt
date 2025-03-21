project(Flecs)

set(FLECS_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    Flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG master
)
FetchContent_MakeAvailable(Flecs)

