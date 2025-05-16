cmake_minimum_required(VERSION 3.21.0)
set(CMAKE_CXX_STANDARD 20)

project(Minesweeper)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
add_subdirectory(src)
add_subdirectory(dependencies)
