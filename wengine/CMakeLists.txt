cmake_minimum_required(VERSION 3.28.1)

project(wen_engine LANGUAGES CXX)

# ---------------------------------------------------
# Vulkan
# ---------------------------------------------------
if (WIN32)
    set(VULKAN_SDK $ENV{VULKAN_SDK})
    set(VULKAN_INCLUDE_DIR ${VULKAN_SDK}/Include)
    set(VULKAN_LIBRARY ${VULKAN_SDK}/Lib/vulkan-1.lib)
    set(GLSLC_EXECUTABLE ${VULKAN_SDK}/Bin/glslc.exe)
else ()
    message(FATAL_ERROR "Unsupported Platform!")
endif ()

if (NOT EXISTS ${VULKAN_LIBRARY})
    message(FATAL_ERROR "Vulkan library not found: ${VULKAN_LIBRARY}")
endif ()

find_library(VULKAN_LIBRARY vulkan HINTS ${CMAKE_LIBRARY_PATH})
find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h HINTS ${CMAKE_INCLUDE_PATH})

# ---------------------------------------------------
# Third-party libraries
# ---------------------------------------------------
macro(fetch_dep name repo tag)
    if (NOT TARGET ${name})
        FetchContent_Declare(
                ${name}
                GIT_REPOSITORY ${repo}
                GIT_TAG ${tag}
        )
        FetchContent_MakeAvailable(${name})
    endif ()
endmacro()

set(GLSLANG_TESTS OFF CACHE BOOL "GLSLANG disable tests" FORCE)
set(ENABLE_OPT 0 CACHE BOOL "Disable SPIR-V optimization tools" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "SPIRV disable executables" FORCE)
set(SPIRV_SKIP_TESTS ON CACHE BOOL "SPIRV disable tests" FORCE)
set(SPIRV_CHECK_CONTEXT OFF CACHE BOOL "SPIRV disable check context" FORCE)
set(SPIRV_HEADERS_ENABLE_TESTS OFF CACHE BOOL "SPIRV Headers disable tests" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "Shaderc disable tests" FORCE)
set(SHADERC_SKIP_EXAMPLES OFF CACHE BOOL "Shaderc disable examples" FORCE)

# TODO: Use this fetch_dep(vulkan-headers https://github.com/KhronosGroup/Vulkan-Headers.git main)
fetch_dep(glslang https://github.com/KhronosGroup/glslang.git main)
fetch_dep(spirv-tools https://github.com/KhronosGroup/SPIRV-Tools.git main)
fetch_dep(spirv-headers https://github.com/KhronosGroup/SPIRV-Headers.git main)
fetch_dep(vk_mem_alloc https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git master)
fetch_dep(shaderc https://github.com/google/shaderc.git main)

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
target_link_libraries(${PROJECT_NAME} PUBLIC
        glfw
        glm
        spdlog::spdlog
        SPIRV-Headers::SPIRV-Headers
        glslang
        GPUOpen::VulkanMemoryAllocator
        shaderc
)

if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
endif ()

# ---------------------------------------------------
# Precompiled headers (PCH)
# ---------------------------------------------------
target_precompile_headers(${PROJECT_NAME} PUBLIC "source/privates/wen_pch.hpp")

# ---------------------------------------------------
# Asset Copy
# ---------------------------------------------------
set(ASSET_SRC_DIR "${CMAKE_SOURCE_DIR}/etc")
set(ASSET_DST_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/etc")
add_custom_command(
        OUTPUT ${ASSET_DST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SRC_DIR} ${ASSET_DST_DIR}
        COMMENT "Copying shader assets to bin folder..."
)
add_custom_target(copy_assets ALL DEPENDS ${ASSET_DST_DIR})
add_dependencies(${PROJECT_NAME} copy_assets)