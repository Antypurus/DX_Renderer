cmake_minimum_required(VERSION 3.14)

project(DX_Renderer VERSION 0.0.1)

set(warnings "/W4 /WX /EHsc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd26812")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

file(GLOB_RECURSE  SRCS
        "DX_Renderer/*.cpp"
        "DX_Renderer/*.hpp"
	"DX_Renderer/*.h")

add_compile_definitions(WIN32_LEAN_AND_MEAN)

add_executable(${CMAKE_PROJECT_NAME} WIN32 DX_Renderer/main.cpp ${SRCS})

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(DX_Renderer PRIVATE 
        "d3d12.lib" 
        "dxgi.lib" 
        "d3dcompiler.lib" 
        "windowscodecs.lib"
        "dxcompiler.lib")
		
add_custom_command(
	TARGET DX_Renderer PRE_BUILD #Adds a post-build event
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${PROJECT_SOURCE_DIR}/DX_Renderer/ThirdParty/DXC/dxcompiler.dll"
	$<TARGET_FILE_DIR:DX_Renderer>)
	
add_custom_command(
	TARGET DX_Renderer PRE_BUILD #Adds a post-build event
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${PROJECT_SOURCE_DIR}/DX_Renderer/ThirdParty/DXC/dxil.dll"
	$<TARGET_FILE_DIR:DX_Renderer>) 
