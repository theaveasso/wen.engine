cmake_minimum_required(VERSION 3.28)


project(Wen.Engine)

find_package(SDL3       CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf   CONFIG REQUIRED)

add_executable(${PROJECT_NAME} source/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 )
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)