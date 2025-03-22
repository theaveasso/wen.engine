cmake_minimum_required(VERSION 3.28.1)

project(wen_editor LANGUAGES CXX)

file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
set(WEN_EDITOR_SOURCES ${SOURCES} ${HEADERS})

add_library(${PROJECT_NAME} SHARED ${WEN_EDITOR_SOURCES})

target_include_directories(
        ${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/flecs
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/imgui
)

target_link_libraries(${PROJECT_NAME} PRIVATE wen_engine SDL3::SDL3 flecs imgui)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
