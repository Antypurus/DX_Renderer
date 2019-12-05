#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"
#include "Core/Components/Fence.hpp"
#include "Core/Components/Command List/GraphicsCommandList.hpp"
#include "Core/Components/Swapchain.hpp"
#include "Tooling/Log.hpp"
#include <thread>
#include "Core/Components/Vertices/VertexBuffer.hpp"
#include "Core/Components/Vertices/IndexBuffer.h"

void MainDirectXThread(DXR::Window& window)
{
	SUCCESS_LOG(L"Main DirectX12 Thread Started");

	DXR::GraphicsDevice device(0);
	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandList commandList = device.CreateGraphicsCommandList();
	DXR::Swapchain swapchain = device.CreateSwapchain(window, 60, commandList);

	DXR::VertexBuffer<DXR::Vertex> vertex_buffer(device, commandList,
										{
													{{1,2,3}},
													{{1,2,3}}
												});
	DXR::IndexBuffer index_buffer(device,commandList,{1,2,3});

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