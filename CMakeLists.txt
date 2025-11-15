cmake_minimum_required(VERSION 3.28)

project(Wen.Engine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_EXTENSIONS ON)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DWEN_ENABLE_LOGGING)
endif()

ADD_SUBDIRECTORY(wen.engine)
ADD_SUBDIRECTORY(wen.sandbox)

# find_package(SDL3       CONFIG REQUIRED)
# find_package(SDL3_image CONFIG REQUIRED)
# find_package(SDL3_ttf   CONFIG REQUIRED)

# add_executable(${PROJECT_NAME}
#   source/main.cpp
#   source/Application.cpp)

# target_link_libraries(${PROJECT_NAME} PRIVATE
#   SDL3::SDL3
#   $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
#   SDL3_ttf::SDL3_ttf)
