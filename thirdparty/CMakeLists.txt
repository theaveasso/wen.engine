project(thirdparty)

include("../cmake/CommonMacros.txt")

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(TF_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ENABLE_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_SAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(JSON_MultipleHeaders ON CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)


fetch_dep(glfw https://github.com/glfw/glfw.git 3.4)
fetch_dep(spdlog https://github.com/gabime/spdlog.git v1.x)
fetch_dep(vkb https://github.com/charles-lunarg/vk-bootstrap main)
fetch_dep(glm https://github.com/g-truc/glm.git 1.0.1)
fetch_dep(volk https://github.com/zeux/volk.git 1.4.304)
fetch_dep(vma https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git v3.2.1)
fetch_dep(taskflow https://github.com/taskflow/taskflow.git master)
fetch_dep(imgui https://github.com/ocornut/imgui.git docking)
fetch_dep(stb https://github.com/nothings/stb.git f0569113c93ad095470c54bf34a17b36646bbbb5)
fetch_dep(nlohmann https://github.com/nlohmann/json.git v3.12.0)
fetch_dep(fmt https://github.com/fmtlib/fmt.git 11.1.4)

add_library(
        imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

add_library(stb INTERFACE ${stb_SOURCE_DIR})
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})