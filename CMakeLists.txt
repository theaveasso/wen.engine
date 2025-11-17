cmake_minimum_required(VERSION 3.28)

project(Wen.Engine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_EXTENSIONS ON)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DWEN_ENABLE_LOGGING)
endif()

add_subdirectory(wen.engine)
add_subdirectory(wen.sandbox)
# find_package(glad CONFIG REQUIRED)
# find_package(spdlog CONFIG REQUIRED)
# find_package(SDL3 CONFIG REQUIRED)

# add_executable(${PROJECT_NAME} src/main.cpp)

# target_link_libraries(${PROJECT_NAME} PRIVATE WenEngine)