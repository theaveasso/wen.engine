cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wengine LANGUAGES C CXX)

set(CMAKE_COLOR_MAKEFILE OFF)
set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE})
set(PROJECT_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)

add_subdirectory(thirdparty/glm EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/vulkan EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/glfw EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/spdlog EXCLUDE_FROM_ALL)

add_subdirectory(wengine)
#add_subdirectory(editor)
add_subdirectory(sandbox)
