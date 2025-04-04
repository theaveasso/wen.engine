project(Spdlog)

set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "Build spdlog as a shared library" FORCE)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "Disable example" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "Disable benchmarks" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "Disable install" FORCE)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)