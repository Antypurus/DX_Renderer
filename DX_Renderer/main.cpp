#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	DXR::GraphicsDevice device = { 1 };
	
	DXR::Window window{ hInstance,nCmdShow,{1280,720},"DX Renderer" };
	while(window.ShouldContinue)
	{
		window.UpdateWindow();
	}
	return 0;
}