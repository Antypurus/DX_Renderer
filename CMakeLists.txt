cmake_minimum_required(VERSION 3.14)

project(DX_Renderer VERSION 0.0.1)

set(warnings "/W4 /WX /EHsc")

file(GLOB_RECURSE  SRCS
        "DX_Renderer/Core/*.cpp"
        "DX_Renderer/Core/*.hpp")

add_executable(${CMAKE_PROJECT_NAME} WIN32 DX_Renderer/main.cpp ${SRCS})

#include_directories("DX_Renderer/Core")

target_link_libraries(DX_Renderer PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
