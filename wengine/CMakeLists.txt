cmake_minimum_required(VERSION 3.28.1)

project(wen_engine LANGUAGES CXX)

file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
set(WEN_SOURCES ${SOURCES} ${HEADERS})

add_library(${PROJECT_NAME} SHARED ${WEN_SOURCES})

target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/flecs
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 flecs )
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)