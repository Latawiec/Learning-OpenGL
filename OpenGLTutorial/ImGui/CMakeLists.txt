cmake_minimum_required(VERSION 3.9)

project(ImGui)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_library(${PROJECT_NAME}
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    imgui::imgui
    glfw
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

target_include_directories(${PROJECT_NAME}
PUBLIC
	Include
)