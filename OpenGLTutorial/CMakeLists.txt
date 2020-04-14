cmake_minimum_required(VERSION 3.9)
project(OpenGLTutorial)

add_subdirectory(Shaders)
add_subdirectory(Textures)
add_subdirectory(Models)
add_subdirectory(ImGui)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
)

if (WIN32) 
set(SYSTEM_LIBS)
else()

# Linux workaround for https://github.com/microsoft/vcpkg/issues/9918
find_library(ASSIMP_ZLIB_LIBRARY z)
find_library(ASSIMP_IRRXML_LIBRARY IrrXML)

message("ASSIMP_ZLIB_LIBRARY   = " "${ASSIMP_ZLIB_LIBRARY}")
message("ASSIMP_IRRXML_LIBRARY = " "${ASSIMP_IRRXML_LIBRARY}")

set(SYSTEM_LIBS ${ASSIMP_ZLIB_LIBRARY} ${ASSIMP_IRRXML_LIBRARY})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Shaders
    Textures
    Models
    ImGui
    glfw
    OpenGL::GL
    glad::glad
    assimp::assimp
    imgui::imgui
    ${SYSTEM_LIBS}
)

add_dependencies(${PROJECT_NAME}
    Shaders
    Textures
    Models
    ImGui
)

target_include_directories(${PROJECT_NAME} PRIVATE
	OpenGL::GL
)