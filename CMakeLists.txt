cmake_minimum_required(VERSION 3.28)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")  # Add this line

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(WenVK CXX C)

include(FetchContent)

add_subdirectory(thirdparty)
add_subdirectory(source)
add_subdirectory(examples/hello_world)

# ---------------------------------------------------
# Compile shaders Target
# ---------------------------------------------------
set(VULKAN_SDK $ENV{VULKAN_SDK})
set(GLSLC_EXECUTABLE ${VULKAN_SDK}/Bin/glslc.exe)

set(SHADERS_SRC_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_DST_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>/shaders")
file(GLOB_RECURSE GLSL_SHADERS "${SHADERS_SRC_DIR}/*.glsl")

set(SPIRV_OUTPUTS "")

foreach (SHADER ${GLSL_SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV_FILE "${SHADERS_DST_DIR}/${FILE_NAME}.spv")
    if (FILE_NAME MATCHES "\\.common\\.")
        continue()
    elseif (FILE_NAME MATCHES "\\.Common\\.")
        continue()
    elseif (FILE_NAME MATCHES "\\.vert\\.")
        set(SHADER_STAGE "vert")
    elseif (FILE_NAME MATCHES "\\.frag\\.")
        set(SHADER_STAGE "frag")
    elseif (FILE_NAME MATCHES "\\.comp\\.")
        set(SHADER_STAGE "comp")
    else ()
        message(FATAL_ERROR "Unknown shader stage for ${FILE_NAME}")
    endif ()
    message(STATUS "Compiled output" ${SPIRV_FILE})
    add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=${SHADER_STAGE} ${SHADER} -o ${SPIRV_FILE}
            COMMENT "Compiling ${FILE_NAME} to SPIR-V..."
            VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV_FILE})
endforeach ()

add_custom_target(CompileShader ALL DEPENDS ${SPIRV_OUTPUTS})