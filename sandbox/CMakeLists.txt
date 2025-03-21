cmake_minimum_required(VERSION 3.30)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wen_sandbox LANGUAGES CXX)

include(FetchContent)

file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
set(WEN_SOURCES ${SOURCES} ${HEADERS})

include_directories("include")

add_executable(${PROJECT_NAME} ${WEN_SOURCES})

target_link_libraries("${PROJECT_NAME}" PRIVATE wen_engine flecs SDL3::SDL3)
