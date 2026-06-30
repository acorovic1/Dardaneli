cmake_minimum_required(VERSION 3.20)
project(Dardaneli LANGUAGES C CXX)


# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#add_definitions(-DNOMINMAX)

# Source files
file(GLOB_RECURSE PROJECT_SOURCES
    src/*.c
    src/*.cpp

    src/*.vert
    src/*.frag
    src/*.geom
    src/*.comp

    libraries/include/ImGUI/*.c
    libraries/include/ImGUI/*.cpp

    libraries/include/ImGUIZMO/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AccelerationStructures
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Objects
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Materials
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLBuffers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Core
)



# Create executable
add_executable(Dardaneli ${PROJECT_SOURCES})


# OpenGL
find_package(OpenGL REQUIRED)

target_link_libraries(Dardaneli PRIVATE  OpenGL::GL)

# GLFW (precompiled)
target_link_libraries(Dardaneli PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/lib/glfw3.lib
)

# Windows system libraries required by GLFW
if (WIN32)
    target_link_libraries(Dardaneli PRIVATE
        opengl32
        user32
        gdi32
        shell32
    )
endif()


# Compiler warnings
if (MSVC)
    target_compile_options(Dardaneli PRIVATE 
    /W4 
    /MP 
    /bigobj 
    /wd4201   # GLM warning
    /permissive-
    )
else()
    target_compile_options(Dardaneli PRIVATE -Wall -Wextra -pedantic)
endif()
