cmake_minimum_required(VERSION 3.9)

project(Textures)

add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE
    TEXTURES_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

