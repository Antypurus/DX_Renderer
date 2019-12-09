#include <Windows.h>
#include <thread>
#include "Tooling/Log.hpp"
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"
#include "Core/Components/Fence.hpp"
#include "Core/Components/Command List/GraphicsCommandList.hpp"
#include "Core/Components/Swapchain.hpp"
#include "Core/Components/Vertices/VertexBuffer.hpp"
#include "Core/Components/Vertices/IndexBuffer.hpp"
#include "Core/Components/Pipeline/PipelineStateObject.hpp"
#include "Core/Components/Shader/VertexShader.hpp"
#include "Core/Components/Shader/PixelShader.hpp"
#include "Core/Components/Resource/ConstantBuffer.hpp"

void MainDirectXThread(DXR::Window& window)
{
	SUCCESS_LOG(L"Main DirectX12 Thread Started");

	DXR::GraphicsDevice device(1);
	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandList commandList = device.CreateGraphicsCommandList();
	DXR::Swapchain swapchain = device.CreateSwapchain(window, 60, commandList);

	DXR::VertexBuffer<DXR::Vertex> vertex_buffer(device, commandList,
												 {
															 {{0,1,10}},
															 {{0.5,0,10}},
															 {{-0.5,0,10}}
												 });
	DXR::IndexBuffer index_buffer(device, commandList, {1,3,2});

	DXR::RootSignature root_signature;
	DXR::DescriptorTableRootParameter desc_table;
	desc_table.AddCBVEntry(1);
	root_signature.AddDescriptorTableRootParameter(desc_table);
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

	using namespace DirectX;
	XMMATRIX mvp = XMMatrixPerspectiveFovLH(XM_PI / 2, 16.0f / 9.0f, 0.1f, 500.0f)*XMMatrixLookAtLH({0,0,0}, {0,0,10}, {0,1,0})*XMMatrixScaling(2000,2000,2000);
	DXR::ConstantBuffer<XMMATRIX> constant_buffer(device, {mvp});

	commandList->Reset(commandList.GetCommandAllocator(), pso.GetPipelineStateObject());

	FLOAT color[4] = {0,1,0,1};
	while(window.ShouldContinue)
	{
		swapchain.Prepare(commandList);

		commandList->ClearRenderTargetView(swapchain.GetCurrentBackBufferDescriptor(), color, 0, nullptr);
		commandList->ClearDepthStencilView(swapchain.GetDepthStencilBufferDescriptor(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		commandList->OMSetRenderTargets(1, &swapchain.GetCurrentBackBufferDescriptor(), TRUE, &swapchain.GetDepthStencilBufferDescriptor());

		ID3D12DescriptorHeap* heaps[] = {constant_buffer.GetDescriptorHeap()->GetRAWInterface()};
		commandList->SetDescriptorHeaps(_countof(heaps), heaps);

		commandList->SetGraphicsRootSignature(root_signature.GetRootSignature());

		commandList->IASetVertexBuffers(0, 1, &vertex_buffer.GetVertexBufferDescriptor());
		commandList->IASetIndexBuffer(&index_buffer.GetIndexBufferDescriptor());
		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootDescriptorTable(0, constant_buffer.GetDescriptorHeap()->Get(0));

		commandList->DrawIndexedInstanced(3, 1, 0, 0,0);
		swapchain.PrepareBackbufferForPresentation(commandList);

		commandList->Close();
		(*device.GetGraphicsCommandQueue())->ExecuteCommandLists(1, commandLists);

		swapchain.Present(commandList);

		device.GetGraphicsCommandQueue()->Flush(fence);

		commandList.GetCommandAllocator()->Reset();
		commandList->Reset(commandList.GetCommandAllocator(), pso.GetPipelineStateObject());
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