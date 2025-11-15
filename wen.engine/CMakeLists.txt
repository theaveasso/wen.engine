project(WenEngine CXX)

find_package(SDL3   CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${PROJECT_NAME} SHARED ${headers} ${sources})

target_compile_definitions(${PROJECT_NAME} PUBLIC -DWEN_EXPORTS)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 spdlog::spdlog)
