cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wen_sandbox LANGUAGES CXX)

include(FetchContent)

file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
set(WEN_SOURCES ${SOURCES} ${HEADERS})

add_executable(${PROJECT_NAME} ${WEN_SOURCES})

target_link_libraries("${PROJECT_NAME}" PRIVATE wen_engine)
