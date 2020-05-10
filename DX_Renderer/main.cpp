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
#include "Core/Components/Resource/Texture/Texture.hpp"
#include "Interface/GUI.hpp"
#include "Core/Components/RayTracing/ShaderBindingTable.hpp"
#include "Core/Components/RayTracing/AccelerationStructure.hpp"
#include "Core/Components/RayTracing/RayTracingPipelineStateObject.hpp"
#include "Core/Components/RayTracing/RayTracingOutput.hpp"
#include "Core/Components/Resource/HeapManager.hpp"
#include "Model Loader/ModelLoader.hpp"
#include "Camera/Camera.hpp"
#include "Core/Components/Motion Estimation/MotionEstimation.hpp"
#include "Voxel/VoxelMap.hpp"
#include "Voxel/Voxelizer.hpp"

__declspec(align(16)) struct CBuffer
{
	DirectX::XMMATRIX mvp;
	DirectX::XMMATRIX voxel;
};

__declspec(align(16)) struct RTCBuffer
{
	DirectX::XMMATRIX voxel_space_matrix;
	DirectX::XMFLOAT3 light_position;
	DirectX::XMFLOAT2 ray_angle_delta;
};

void MainDirectXThread(DXR::Window& window)
{
	SUCCESS_LOG(L"Main DirectX12 Thread Started");

	DXR::GraphicsDevice device;

	DXR::VertexShader vs = DXR::VertexShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/VertexShader.hlsl", L"VSMain");
	DXR::PixelShader ps = DXR::PixelShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/VertexShader.hlsl", L"PSMain");
	DXR::RayGenShader rgs = DXR::RayGenShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/RTX.hlsl", L"raygen");
	DXR::IntersectionShader ints = DXR::IntersectionShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/RTX.hlsl", L"intersection");
	DXR::AnyHitShader ah = DXR::AnyHitShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/RTX.hlsl", L"anyhit");
	DXR::ClosestHitShader chs = DXR::ClosestHitShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/RTX.hlsl", L"closesthit");
	DXR::MissShader ms = DXR::MissShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/RTX.hlsl", L"miss");

	DXR::RootSignature root_signature;

	DXR::DescriptorTableRootParameter cbv_desc_table;
	cbv_desc_table.AddCBVEntry(0);

	DXR::DescriptorTableRootParameter uav_desc_table;
	uav_desc_table.AddUAVEntry(0);

	DXR::DescriptorTableRootParameter srv_desc_table;
	srv_desc_table.AddSRVEntry(0);

	DXR::DescriptorTableRootParameter sampler_desc_table;
	sampler_desc_table.AddSamplerEntry(0);

	DXR::DescriptorTableRootParameter voxel_desc_table;
	voxel_desc_table.AddSRVEntry(0,1);

	root_signature.AddDescriptorTableRootParameter(cbv_desc_table);
	root_signature.AddDescriptorTableRootParameter(uav_desc_table);
	root_signature.AddDescriptorTableRootParameter(srv_desc_table);
	root_signature.AddDescriptorTableRootParameter(sampler_desc_table);
	root_signature.AddDescriptorTableRootParameter(voxel_desc_table);

	DXR::DescriptorRootParameter acceleration_structure_root_parameter(DXR::RootParameterDescriptorType::SRV, 1);
	root_signature.AddDescriptorRootParameter(acceleration_structure_root_parameter);

	root_signature.CreateRootSignature(device);

	DXR::RayTracingPipelineStateObject rtpso = {
		device,root_signature,rgs,ints,ah,chs,ms
	};

	DXR::PipelineStateObject pso = {
		device,
		vs.GetShaderBytecode(),
		ps.GetShaderBytecode(),
		root_signature,
		DXR::OBJVertex::GetInputLayout(),
		DXR::Swapchain::m_backbuffer_format,
		DXR::DepthStencilBuffer::DepthStencilBufferFormat };

	DXR::VertexShader voxelization_vs = DXR::VertexShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/Voxelization.hlsl", L"VoxelVSMain");
	DXR::PixelShader voxelization_ps = DXR::PixelShader::CompileShaderFromFile(L"./DX_Renderer/Resources/Shaders/Voxelization.hlsl", L"VoxelPSMain");

	DXR::PipelineStateObject voxel_pso = {
		device,
		voxelization_vs.GetShaderBytecode(),
		voxelization_ps.GetShaderBytecode(),
		root_signature,
		DXR::OBJVertex::GetInputLayout(),
		DXR::Swapchain::m_backbuffer_format,
		DXR::DepthStencilBuffer::DepthStencilBufferFormat };

	DXR::Fence fence = device.CreateFence(0);
	DXR::GraphicsCommandList commandList = device.CreateGraphicsCommandList();

	commandList.FullReset(pso);
	commandList.SetName(L"Main Command List");

	DXR::Swapchain swapchain = device.CreateSwapchain(window, 60, commandList);

	auto texture = DXR::Texture(L"./DX_Renderer/Resources/Textures/star.jpg", device, commandList);

	commandList->Close();
	device.GetGraphicsCommandQueue().ExecuteCommandList(commandList);
	device.GetGraphicsCommandQueue().Flush(fence);
	commandList.FullReset(pso);

	DXR::Camera cam({ 0,0,-10 }, { 0,0,1 });

	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, 1280.0f / 720.0f, 0.1f, 1000.0f);
	DirectX::XMMATRIX view = cam.ViewMatrix();
	DirectX::XMMATRIX model = DirectX::XMMatrixScaling(0.5, 0.5, 0.5);

	DirectX::XMMATRIX mvp = model * view * projection;
	CBuffer raster_cbufer;
	raster_cbufer.mvp = mvp;
	raster_cbufer.voxel = DirectX::XMMatrixIdentity();
	DXR::ConstantBuffer<CBuffer> constant_buffer(device, { raster_cbufer });

	CBuffer light_buff;
		light_buff.mvp = mvp;
	light_buff.voxel = DirectX::XMMatrixIdentity();
	DXR::ConstantBuffer<CBuffer> light_cbuffer(device, { light_buff });


	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float x_rotation_angle = 0;
	float y_rotation_angle = 0;
	float z_rotation_angle = 0;
	float scale = 0.5;

	auto sib_model = DXR::ModelLoader::LoadOBJ("./DX_Renderer/Resources/Models/sibenik/sibenik.obj", device, commandList);
	auto vertex_buffer = sib_model.GenerateVertexBuffer(device, commandList);
	auto index_buffer = sib_model.GenerateIndexBuffer(device, commandList);

	DXR::GUI gui(device, window, swapchain);

	DXR::RayTracingOutput rt_out(device, commandList, swapchain);

	DXR::BLAS blas(device, commandList, vertex_buffer, index_buffer, true);
	DXR::TLAS tlas;
	tlas.AddInstance(blas, mvp, 0);
	tlas.BuildTLAS(device, commandList);

	DXR::Voxelizer voxelizer(device, commandList, root_signature, sib_model, mvp);
	DXR::VoxelMap light_map(device, 128, 128, 128);
	light_map.voxel_volume_texture->SetName(L"Voxel Irradiance Map");

	RTCBuffer rt_light;
	rt_light.light_position = { 3.5,-4.1,-0.1 };
	rt_light.voxel_space_matrix = voxelizer.voxel_space_conversion_matrix;
	rt_light.ray_angle_delta = { 100.0f,100.0f };

	DXR::ConstantBuffer<RTCBuffer> rtc_buffer(device, { rt_light });
	rtc_buffer->SetName(L"RTX Shading CBuffer");

	DXR::RayGenSBTEntry raygen(rgs);
	raygen.AddResource(light_map.GetGPUHandle());
	raygen.AddResource(rtc_buffer.GetGPUHandle());
	DXR::MissSBTEntry miss(ms);
	DXR::HitGroupSBTEntry hitgroup(L"HitGroup");

	DXR::ShaderBindingTable sbtable(device, rtpso, raygen, miss, hitgroup);

	float x_off = 0;

	UINT count = 0;

	while (window.ShouldContinue)
	{
		// Start the Dear ImGui frame
		gui.StartFrame();

		ImGui::Begin("Window");
		ImGui::SliderAngle("X Rotation", &x_rotation_angle);
		ImGui::SliderAngle("Y Rotation", &y_rotation_angle);
		ImGui::SliderAngle("Z Rotation", &z_rotation_angle);
		ImGui::SliderFloat("Model Scale", &scale, 0, 1);
		ImGui::SliderFloat3("Light Position", (float*)&rt_light.light_position,-10,10);
		ImGui::End();

		{
			///NOTE(Tiago): The view matrix will only update if the camera parameters have changed
			view = cam.ViewMatrix();

			model = DirectX::XMMatrixRotationAxis({ 1.0f,0.0f,0.0f }, x_rotation_angle);
			model *= DirectX::XMMatrixRotationAxis({ 0.0f,1.0f,0.0f }, y_rotation_angle);
			model *= DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, z_rotation_angle);
			model *= DirectX::XMMatrixScaling(scale, scale, scale);
			model *= DirectX::XMMatrixTranslation(x_off, 0, 0);
			raster_cbufer.mvp = model * view * projection;
			raster_cbufer.voxel = voxelizer.voxel_space_conversion_matrix;
			light_buff.mvp = DirectX::XMMatrixTranslation(rt_light.light_position.x,rt_light.light_position.y,rt_light.light_position.z) * view * projection;
			light_buff.voxel = voxelizer.voxel_space_conversion_matrix;
			constant_buffer.UpdateData({ raster_cbufer });
			light_cbuffer.UpdateData({light_buff});
		}

		commandList.SetGraphicsRootSignature(root_signature);
		commandList->SetComputeRootSignature(root_signature.GetRootSignature());//NOTE(Tiago):Placeholder, need to create an lite abstraction to bind the compute root signature, which in this case is used for the RT pipeline
		swapchain.Prepare(commandList);

		swapchain.GetCurrentBackBuffer().Clear(commandList, color);
		swapchain.GetDepthStencilBuffer().Clear(commandList);
		commandList.SetDisplayRenderTarget(swapchain.GetCurrentBackBuffer(), swapchain.GetDepthStencilBuffer());

		commandList.BindDescriptorHeaps({ &DXR::SRHeapManager::GetManager().descriptor_heap, &DXR::SamplerHeapManager::GetManager().descriptor_heap });

		commandList.BindVertexBuffer(vertex_buffer);
		commandList.BindIndexBuffer(index_buffer);

		swapchain.SetViewport(commandList, swapchain.GetBackbufferResolution());
		commandList->SetPipelineState(pso.GetPipelineStateObject());

		commandList.BindConstantBuffer(constant_buffer, 1);
		light_map.BindUAV(commandList, 2);
		light_map.BindSRV(commandList, 5);
		sib_model.Draw(commandList, 3, 4);

		commandList.SendDrawCall();

		//Render Light
		{
			commandList.BindConstantBuffer(light_cbuffer, 1);
			commandList.BindVertexBuffer(*voxelizer.voxel_cube_vertex_buffer);
			commandList.BindIndexBuffer(*voxelizer.voxel_cube_index_buffer);
			commandList.SendDrawCall();
		}

		commandList.BindVertexBuffer(vertex_buffer);
		commandList.BindIndexBuffer(index_buffer);

		commandList.BindConstantBuffer(constant_buffer, 1);
		commandList.BindTexture(texture, 3, 4);

		voxelizer.Voxelize(commandList, cam, root_signature, model, 1, 2);

		voxelizer.BuildAccelerationStructure(device, commandList, fence);

		swapchain.SetViewport(commandList, swapchain.GetBackbufferResolution());

		rt_light.voxel_space_matrix = voxelizer.voxel_space_conversion_matrix;
		rtc_buffer.UpdateData({ rt_light });
		{
			if (voxelizer.acceleration_structure.GetInstanceCount() != 0)
			{
				commandList->SetPipelineState1(rtpso.GetRTPSO());

				commandList.BindTLAS(voxelizer.acceleration_structure, 0);
				//rt_out.Bind(commandList, 2);
				light_map.Clear(commandList);
				light_map.BindComputeUAV(commandList, 2);
				commandList.BindComputeConstantBuffer(rtc_buffer, 1);

				D3D12_DISPATCH_RAYS_DESC rays = {};

				rays.HitGroupTable.StartAddress = sbtable.GetHitGroupEntryAddress();
				rays.HitGroupTable.StrideInBytes = sbtable.GetHitGroupSectionSize();
				rays.HitGroupTable.SizeInBytes = sbtable.GetHitGroupEntrySize();

				rays.MissShaderTable.StartAddress = sbtable.GetMissEntryAddress();
				rays.MissShaderTable.StrideInBytes = sbtable.GetMissEntrySize();
				rays.MissShaderTable.SizeInBytes = sbtable.GetMissEntrySize();

				rays.RayGenerationShaderRecord.StartAddress = sbtable.GetRayGenEntryAddress();
				rays.RayGenerationShaderRecord.SizeInBytes = sbtable.GetRayGenEntrySize();

				rays.Depth = 1280;
				rays.Width = 720;//swapchain.GetBackbufferResolution().Width;
				rays.Height = 1;//swapchain.GetBackbufferResolution().Height;
				commandList->DispatchRays(&rays);

				//rt_out.CopyToBackbuffer(commandList,swapchain);
			}
		}

		//voxelizer.Voxelize(cam);

		gui.Render(commandList);

		swapchain.PrepareBackbufferForPresentation(commandList);

		commandList.Close();
		device.GetGraphicsCommandQueue().ExecuteCommandList(commandList);

		swapchain.Present();

		device.GetGraphicsCommandQueue().Flush(fence);

		commandList.FullReset(pso);
	}

}

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	DXR::Window window{ hInstance,nCmdShow,{1920,1080},"DX Renderer" };

	std::thread main_dx12_thread(MainDirectXThread, std::ref(window));

	while (window.ShouldContinue)
	{
		window.UpdateWindow();
	}

	main_dx12_thread.join();
	return 0;
}