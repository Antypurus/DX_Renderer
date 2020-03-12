#include <iostream>
#include <Windows.h>
#include <thread>
#include "Tooling/Log.hpp"
#include "Core/Components/Command Queue/GraphicsCommandQueue.hpp"
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
#include "Core/Components/Resource/GPU Buffers/ConstantBuffer.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_win32.h"
#include "ThirdParty/imgui/imgui_impl_dx12.h"

void MainDirectXThread(DXR::Window& window)
{
	SUCCESS_LOG(L"Main DirectX12 Thread Started");

	DXR::GraphicsDevice device;

	DXR::VertexShader vs = DXR::VertexShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/VertexShader.hlsl", "VSMain");
	DXR::PixelShader ps = DXR::PixelShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/VertexShader.hlsl", "PSMain");

	DXR::RootSignature root_signature;
	DXR::DescriptorTableRootParameter desc_table;
	desc_table.AddCBVEntry(1);
	root_signature.AddDescriptorTableRootParameter(desc_table);
	root_signature.CreateRootSignature(device);

	DXR::PipelineStateObject pso = {
		device,
		vs.GetShaderBytecode(),
		ps.GetShaderBytecode(),
		root_signature,
		DXR::Vertex::GetInputLayout(),
		DXR::Swapchain::m_backbuffer_format,
		DXR::DepthStencilBuffer::DepthStencilBufferFormat};

	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandList commandList = device.CreateGraphicsCommandList();

	commandList.FullReset(pso);
	commandList.SetName(L"Main Command List");

	DXR::Swapchain swapchain = device.CreateSwapchain(window, 60, commandList);
	DXR::VertexBuffer<DXR::Vertex> vertex_buffer(device, commandList,
												 {
													 {{-1.0f, -1.0f,  1.0f},{1.0f,1.0f,1.0f,1.0f}},
													{{1.0f, -1.0f,  -1.0f},{0.0f,1.0f,0.0f,1.0f}},
													{{1.0f, -1.0f,  1.0f},{1.0f,0.0f,0.0f,1.0f}},
													{{-1.0f, -1.0f,  -1.0f},{0.0f,0.0f,1.0f,1.0f}},
													{{0.0f, 1.0f,  0.0f},{0.0f,0.0f,0.0f,1.0f}},
												 });
	DXR::IndexBuffer index_buffer(device, commandList, 
									{   0,2,1,
										0,1,3,
										0,2,4,
										2,1,4,
										1,3,4,
										3,0,4
								  });

	commandList->Close();
	
	device.GetGraphicsCommandQueue().ExecuteCommandList(commandList);
	device.GetGraphicsCommandQueue().Flush(fence);

	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, 1280.0f / 720.0f, 0.1f, 1000.0f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH({0.0f,0.0f,-10.0f,1.0f}, {0.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f,0.0f,0.0f});
	DirectX::XMMATRIX model = DirectX::XMMatrixScaling(1, 1, 1);

	DirectX::XMMATRIX mvp = model * view * projection;
	DXR::ConstantBuffer<DirectX::XMMATRIX> constant_buffer(device, {mvp});

	commandList.FullReset(pso);

	FLOAT color[4] = {0.4f, 0.6f, 0.9f, 1.0f};

	float scale_factor = 1;
	float scale_step = 0.1f;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ID3D12DescriptorHeap* heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)) != S_OK)
		return;

	ImGui_ImplWin32_Init(window.GetCurrentWindowHandle());
	ImGui_ImplDX12_Init(device.GetRawInterface(), 2,
		swapchain.m_backbuffer_format, heap,
		heap->GetCPUDescriptorHandleForHeapStart(),
		heap->GetGPUDescriptorHandleForHeapStart());


	while(window.ShouldContinue)
	{

		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{
			scale_factor += scale_step;
			model = DirectX::XMMatrixRotationAxis({0.0f,1.0f,0.0f},scale_factor);
			mvp = model * view * projection;
			constant_buffer.UpdateData({mvp});
		}

		commandList.SetGraphicsRootSignature(root_signature);
		swapchain.Prepare(commandList);

		swapchain.GetCurrentBackBuffer().Clear(commandList, color);
		swapchain.GetDepthStencilBuffer().Clear(commandList);
		commandList.SetDisplayRenderTarget(swapchain.GetCurrentBackBuffer(), swapchain.GetDepthStencilBuffer());
		
		commandList.BindDescriptorHeap(*constant_buffer.GetDescriptorHeap());

		commandList.BindVertexBuffer(vertex_buffer);
		commandList.BindIndexBuffer(index_buffer);
		commandList.BindConstantBuffer(constant_buffer, 0);

		commandList.SendDrawCall();

		ID3D12DescriptorHeap* heaps[] = { heap };
		commandList->SetDescriptorHeaps(1, heaps);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.GetRAWInterface());
		
		commandList.SendDrawCall();

		swapchain.PrepareBackbufferForPresentation(commandList);

		commandList.Close();
		device.GetGraphicsCommandQueue().ExecuteCommandList(commandList);

		swapchain.Present();

		device.GetGraphicsCommandQueue().Flush(fence);

		commandList.FullReset(pso);
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

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