project(Flecs)

set(FLECS_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG master
)
FetchContent_MakeAvailable(flecs)