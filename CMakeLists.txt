cmake_minimum_required(VERSION 3.28)

project(Wen.Engine)

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3       CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf   CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
  source/main.cpp
  source/Application.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  SDL3::SDL3
  $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
  SDL3_ttf::SDL3_ttf)