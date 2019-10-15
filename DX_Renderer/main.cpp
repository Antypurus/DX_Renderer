#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	DXR::Window window{ hInstance,nCmdShow,{1280,720},"DX Renderer" };
	while(window.ShouldContinue)
	{
		window.UpdateWindow();
	}
	return 0;
}