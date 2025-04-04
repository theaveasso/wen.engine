cmake_minimum_required(VERSION 3.28.1)

project(wen_engine LANGUAGES CXX)

# vulkan
set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_INCLUDE_DIR ${VULKAN_SDK}/Include)
set(VULKAN_LIBRARY ${VULKAN_SDK}/Lib/vulkan-1.lib)
if (NOT EXISTS ${VULKAN_LIBRARY})
    message(FATAL_ERROR "Vulkan library not found: ${VULKAN_LIBRARY}")
endif ()

find_library(VULKAN_LIBRARY vulkan HINTS ${CMAKE_LIBRARY_PATH})
find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS ${CMAKE_INCLUDE_PATH})

FetchContent_Declare(
        vk_mem_alloc
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG master
)
FetchContent_MakeAvailable(vk_mem_alloc)

file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)
set(WEN_SOURCES ${SOURCES})

add_library(${PROJECT_NAME} SHARED ${WEN_SOURCES})

target_include_directories(
        ${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/glfw
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/spdlog
        ${VULKAN_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${VULKAN_LIBRARY})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw glm spdlog::spdlog GPUOpen::VulkanMemoryAllocator)

target_precompile_headers(${PROJECT_NAME} PUBLIC "source/privates/wen_pch.hpp")

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
endif ()

set(ASSET_SRC_DIR "${CMAKE_SOURCE_DIR}/etc")
set(ASSET_DST_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/etc")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_SRC_DIR} ${ASSET_DST_DIR}
        COMMENT "Copying shader assets to bin folder..."
)