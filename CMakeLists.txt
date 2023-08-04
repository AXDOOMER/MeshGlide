cmake_minimum_required(VERSION 3.12)
project(MeshGlide)

set(CMAKE_CXX_STANDARD 14)

# Include vcpkg toolchain file
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Adjust the path to your CPP files
set(SOURCE_FILES Projects/main.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)

add_executable(MeshGlide ${SOURCE_FILES})
target_link_libraries(MeshGlide PRIVATE glfw SDL2::SDL2 SDL2::SDL2_image cppzmq)