project(HelloWorld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20 0N)

add_executable(${PROJECT_NAME}
        hw_game.cpp
        hello_world.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE WVkEngine)
