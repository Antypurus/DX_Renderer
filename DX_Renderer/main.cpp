#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"
#include "Core/Components/Fence.hpp"
#include "Core/Components/Command List/GraphicsCommandList.hpp"
#include "Core/Components/Swapchain.hpp"
#include "Core/Components/Resource/DescriptorHeap.hpp"
#include "Tooling/Log.hpp"

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	DXR::Window window{hInstance,nCmdShow,{1280,720},"DX Renderer"};

	DXR::GraphicsDevice device;
	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandList commandList =  device.CreateGraphicsCommandList();
	DXR::Swapchain swapchain = device.CreateSwapchain(window,60);
	//DXR::DescriptorHeap heap = device.CreateRenderTargetViewDescriptorHeap(2);
	
	while(window.ShouldContinue)
	{
		//swapchain->Present(0,0);
		window.UpdateWindow();
	}
	return 0;
}