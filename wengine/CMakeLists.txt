cmake_minimum_required(VERSION 3.30)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wenengine LANGUAGES CXX)

include(FetchContent)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/sdl3 ${CMAKE_BINARY_DIR}/thirdparty/sdl3)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/flecs ${CMAKE_BINARY_DIR}/thirdparty/flecs)

file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
set(WEN_SOURCES ${SOURCES} ${HEADERS})

include_directories("include")

add_library(${PROJECT_NAME} SHARED ${WEN_SOURCES})

target_link_libraries("${PROJECT_NAME}" PRIVATE SDL3::SDL3 flecs::flecs)
