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

# TODO: Use this fetch_dep(vulkan-headers https://github.com/KhronosGroup/Vulkan-Headers.git main)
fetch_dep(vk_mem_alloc https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git master)

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
        GPUOpen::VulkanMemoryAllocator
)

if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
endif ()

# ---------------------------------------------------
# Precompiled headers (PCH)
# ---------------------------------------------------
target_precompile_headers(${PROJECT_NAME} PUBLIC "source/privates/wen_pch.hpp")

# ---------------------------------------------------
# Compile shaders Target
# ---------------------------------------------------
set(SHADERS_SRC_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_DST_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/shaders")
file(GLOB_RECURSE GLSL_SHADERS "${SHADERS_SRC_DIR}/*.glsl")

set(SPIRV_OUTPUTS "")

foreach (SHADER ${GLSL_SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV_FILE "${SHADERS_DST_DIR}/${FILE_NAME}.spv")
    if (FILE_NAME MATCHES "\\.vert\\.")
        set(SHADER_STAGE "vert")
    elseif (FILE_NAME MATCHES "\\.frag\\.")
        set(SHADER_STAGE "frag")
    else ()
        message(FATAL_ERROR "Unknown shader stage for ${FILE_NAME}")
    endif ()
    add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=${SHADER_STAGE} ${SHADER} -o ${SPIRV_FILE}
            COMMENT "Compiling ${FILE_NAME} to SPIR-V..."
            VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV_FILE})
endforeach ()

add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_OUTPUTS})