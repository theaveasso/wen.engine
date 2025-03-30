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
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/glm
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC  flecs glm)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if (NOT MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE WEN_PLATFORM_DESKTOP_SDL)
target_compile_definitions(${PROJECT_NAME} PRIVATE WEN_BUILD_SHARED)

set(ASSET_SRC_DIR "${CMAKE_SOURCE_DIR}/etc")
set(ASSET_DST_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/etc")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSET_SRC_DIR} ${ASSET_DST_DIR}
    COMMENT "Copying shader assets to bin folder..."
)

