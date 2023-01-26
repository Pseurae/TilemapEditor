cmake_minimum_required(VERSION 3.16)
project(stb_image)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} stb_image.c)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC .)

