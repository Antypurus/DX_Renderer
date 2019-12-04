#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"
#include "Core/Components/Fence.hpp"
#include "Core/Components/Command List/GraphicsCommandList.hpp"
#include "Core/Components/Swapchain.hpp"
#include "Tooling/Log.hpp"
#include <thread>
#include "Core/Components/Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "Core/Components/Resource/GPU Buffers/GPUUploadBuffer.hpp"

void MainDirectXThread(DXR::Window& window)
{
	SUCCESS_LOG(L"Main DirectX12 Thread Started");

	DXR::GraphicsDevice device;
	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandList commandList = device.CreateGraphicsCommandList();
	DXR::Swapchain swapchain = device.CreateSwapchain(window, 60, commandList);

	DXR::GPUUploadBuffer upload(device, 5, 256);
	DXR::GPUDefaultBuffer buffer(device, commandList, 5, 256);

	commandList->Close();
	ID3D12CommandList* commandLists[] = {commandList.GetRAWInterface()};
	(*device.GetGraphicsCommandQueue())->ExecuteCommandLists(1, commandLists);
	device.GetGraphicsCommandQueue()->Flush(fence);

	while(window.ShouldContinue)
	{
		swapchain.Present(commandList);
	}

}

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
							LPSTR lpCmdLine, int nCmdShow)
{
	DXR::Window window{hInstance,nCmdShow,{1280,720},"DX Renderer"};

	std::thread main_dx12_thread(MainDirectXThread, std::ref(window));

	while(window.ShouldContinue)
	{
		window.UpdateWindow();
	}

	main_dx12_thread.join();
	return 0;
}