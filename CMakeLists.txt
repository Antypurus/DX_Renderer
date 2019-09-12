cmake_minimum_required(VERSION 3.14)

project(DX_Renderer VERSION 0.0.1)

add_executable(${CMAKE_PROJECT_NAME} WIN32 src/main.cpp)

# Linking with D3D12 components
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
