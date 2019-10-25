cmake_minimum_required(VERSION 3.14)

project(DX_Renderer VERSION 0.0.1)

set(warnings "/W4 /WX /EHsc")

file(GLOB_RECURSE  SRCS
        "DX_Renderer/*.cpp"
        "DX_Renderer/*.hpp")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(DEBUG=1)
endif()

add_executable(${CMAKE_PROJECT_NAME} WIN32 DX_Renderer/main.cpp ${SRCS})

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(DX_Renderer PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")