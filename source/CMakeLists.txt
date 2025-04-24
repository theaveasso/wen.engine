cmake_minimum_required(VERSION 3.28.1)

project(WVkEngine CXX)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES_FILES "wvk/*.cpp")
file(GLOB_RECURSE HEADERS_FILES "wvk/*.hpp")

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

include_directories(wvk)
add_library(${PROJECT_NAME}
        SHARED
        ${SOURCES_FILES}
        ${HEADERS_FILES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

target_precompile_headers(${PROJECT_NAME}
        PUBLIC
        "wvk/common/wvk_pch.hpp")

target_link_libraries(${PROJECT_NAME}
        PUBLIC

        # vulkan
        volk::volk_headers
        vk-bootstrap
        vk-bootstrap-compiler-warnings
        GPUOpen::VulkanMemoryAllocator

        glm::glm
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        stb
        imgui
        Tracy::TracyClient

        PRIVATE
        glfw)

#engine
target_compile_definitions(
        ${PROJECT_NAME}
        PRIVATE
        SPDLOG_COMPILED_LIB
        WVkEngine_EXPORTS)

# volk
target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        VK_NO_PROTOTYPES
        VMA_VULKAN_VERSION=1003000)

# glm
target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        GLM_FORCE_CTOR_INIT
        GLM_FORCE_XYZW_ONLY
        GLM_FORCE_EXPLICIT_CTOR
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_ENABLE_EXPERIMENTAL
        GLM_FORCE_RADIANS)

# tracy
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(
            ${PROJECT_NAME}
            PUBLIC
            TRACY_ENABLE
            TRACY_DELAYED_INIT)
endif ()

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zi /EHsc /wd4100 /wd4251 /wd4324)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_KHR_buffer_device_address)
endif ()

