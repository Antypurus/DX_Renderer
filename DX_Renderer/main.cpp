#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"
#include "Core/Components/Fence.hpp"
#include "Core/Components/Command List/GraphicsCommandList.hpp"
#include "Core/Components/Swapchain.hpp"
#include "Tooling/Log.hpp"
#include <thread>
#include "Core/Components/Vertices/VertexBuffer.hpp"
#include "Core/Components/Vertices/IndexBuffer.hpp"
#include "Core/Components/Resource/ConstantBuffer.hpp"
#include "Core/Components/Shader/Root Signature/RootSignature.hpp"
#include "Core/Components/Pipeline/PipelineStateObject.hpp"
#include "Core/Components/Shader/VertexShader.hpp"
#include "Core/Components/Shader/PixelShader.hpp"

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
	DXR::IndexBuffer index_buffer(device, commandList, {1,2,3});
	DXR::ConstantBuffer<DirectX::XMMATRIX> constant_buffer(device, {
		{
			1,2,3,4,
			1,2,3,4,
			1,2,3,4,
			1,2,3,4}
														   });

	DXR::RootSignature root_signature;
	DXR::ConstantRootParameter constant_root_parameter(0, 1);
	root_signature.AddConstantRootParameter(constant_root_parameter);
	root_signature.CreateRootSignature(device);

	DXR::VertexShader vs = DXR::VertexShader::CompileShaderFromFile(L"C:/Users/craky/Desktop/DX_Renderer/DX_Renderer/Resources/Shaders/VertexShader.hlsl", "main");
	DXR::PixelShader ps = DXR::PixelShader::CompileShaderFromFile(L"C:/Users/craky/Desktop/DX_Renderer/DX_Renderer/Resources/Shaders/PixelShader.hlsl", "main");

	DXR::PipelineStateObject pso = {
		device,
		vs.GetShaderBytecode(),
		ps.GetShaderBytecode(),
		root_signature,
		vertex_buffer.GetInputLayout(),
		swapchain.m_backbuffer_format,DXR::DepthStencilBuffer::DepthStencilBufferFormat};

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