#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"
#include "Core/Components/Fence.hpp"
#include "Core/Components/Command Queue/GraphicsCommandQueue.hpp"
#include "Core/Components/Command List/GraphicsCommandList.hpp"

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	DXR::GraphicsDevice device;
	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandQueue queue{device};
	DXR::GraphicsCommandList commandList{device};

	DXR::Window window{ hInstance,nCmdShow,{1280,720},"DX Renderer" };
	while(window.ShouldContinue)
	{
		window.UpdateWindow();
	}
	return 0;
}