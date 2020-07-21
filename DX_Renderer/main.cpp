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
#include "ThirdParty/NVAPI/nvapi.h"
#include "ThirdParty/PerfTimers/PerformanceTimers.h"
#include <fstream>

#define ENABLE_UI 1

#define rad(x) (x*DirectX::XM_PI)/180.0f

__declspec(align(16)) struct CBuffer
{
	DirectX::XMMATRIX mvp;
	DirectX::XMMATRIX vp;
	DirectX::XMMATRIX voxel;
};

__declspec(align(16)) struct RTCBuffer
{
	DirectX::XMMATRIX voxel_space_matrix;
	DirectX::XMFLOAT4 light_color;
	DirectX::XMFLOAT3 light_position;
	float light_radius;
	float light_extent;
};

void InitNVAPI()
{
	NvAPI_Status NvapiStatus = NvAPI_Initialize();
    
	if (NvapiStatus != NVAPI_OK)
	{
		printf("NVAPI ERROR %d\n", NvapiStatus);
	}
}

void DeinitNVAPI()
{
	NvAPI_Unload();
}

void MainDirectXThread(DXR::Window& window)
{
	SUCCESS_LOG(L"Main DirectX12 Thread Started");
    
	InitNVAPI();
    
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
	voxel_desc_table.AddSRVEntry(0, 1);
    
	DXR::DescriptorTableRootParameter light_desc_table;
	light_desc_table.AddCBVEntry(0, 1);
    
	DXR::DescriptorTableRootParameter normal_map_desc_table;
	normal_map_desc_table.AddUAVEntry(1);
    
	DXR::DescriptorTableRootParameter timer_uav_desc_table;
	timer_uav_desc_table.AddUAVEntry(10);
    
	root_signature.AddDescriptorTableRootParameter(cbv_desc_table);
	root_signature.AddDescriptorTableRootParameter(uav_desc_table);
	root_signature.AddDescriptorTableRootParameter(srv_desc_table);
	root_signature.AddDescriptorTableRootParameter(sampler_desc_table);
	root_signature.AddDescriptorTableRootParameter(voxel_desc_table);
	root_signature.AddDescriptorTableRootParameter(light_desc_table);
	root_signature.AddDescriptorTableRootParameter(normal_map_desc_table);
	root_signature.AddDescriptorTableRootParameter(timer_uav_desc_table);
    
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
	cam.position = {4.96508f,2.40953f,-0.54187f};
	cam.pitch = -1.150005f;
	cam.yaw = -88.74990f;
	cam.Rotate();
    
    float initial_scale = 0.01f;
    
	double fov = 40;

	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(rad(fov), 1280.0f / 720.0f, 0.1f, 1000.0f);
	DirectX::XMMATRIX view = cam.ViewMatrix();
	DirectX::XMMATRIX model = DirectX::XMMatrixScaling(initial_scale, initial_scale, initial_scale);
    
	DirectX::XMMATRIX mvp = model * view * projection;
	CBuffer raster_cbufer;
	raster_cbufer.mvp = mvp;
	raster_cbufer.voxel = DirectX::XMMatrixIdentity();
	DXR::ConstantBuffer<CBuffer> constant_buffer(device, { raster_cbufer });
    
	CBuffer light_buff;
	light_buff.mvp = mvp;
	light_buff.vp = view * projection;
	light_buff.voxel = DirectX::XMMatrixIdentity();
	DXR::ConstantBuffer<CBuffer> light_cbuffer(device, { light_buff });
    
    
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
	float x_rotation_angle = 0;
	float y_rotation_angle = 0;
	float z_rotation_angle = 0;
	float scale = initial_scale;
    
	auto sib_model = DXR::ModelLoader::LoadOBJ("./DX_Renderer/Resources/Models/sponza/sponza.obj", device, commandList);
	auto vertex_buffer = sib_model.GenerateVertexBuffer(device, commandList);
	auto index_buffer = sib_model.GenerateIndexBuffer(device, commandList);
    
#if ENABLE_UI
	DXR::GUI gui(device, window, swapchain);
#endif    

	DXR::RayTracingOutput rt_out(device, commandList, swapchain);
    
	DXR::BLAS blas(device, commandList, vertex_buffer, index_buffer, true);
	DXR::TLAS tlas;
	tlas.AddInstance(blas, DirectX::XMMatrixScaling(initial_scale, initial_scale, initial_scale), 0);
	tlas.BuildTLAS(device, commandList);
    
	DXR::Voxelizer voxelizer(device, commandList, root_signature, sib_model, mvp);
	DXR::VoxelMap light_map(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, DXR::MapType::R8G8B8A8Unorm, DXR::MapType::R32Uint, false);
	light_map.voxel_volume_texture->SetName(L"Voxel Irradiance Map");
    
	RTCBuffer rt_light;
	rt_light.light_position = { -0.741f, 3.407f, -0.294f };
	rt_light.voxel_space_matrix = DirectX::XMMatrixScaling(1/initial_scale, 1/initial_scale, 1/initial_scale) * voxelizer.voxel_space_conversion_matrix;
	rt_light.light_color = { 1.0f,1.0f,1.0f,1.0f };
	rt_light.light_radius = 0.5f;
	rt_light.light_extent = 0.5f;
    
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
    
	//0->RT
	//1->Voxelization
	//2->Final Pass
	DX::GPUTimer timers[3];
    
	for (unsigned int i = 0; i < 3; ++i)
	{
		timers[i].RestoreDevice(device.GetRawInterface(), device.GetGraphicsCommandQueue().GetCommandQueueRawPtr(), 1);
	}
    
	std::vector<double> rt_times;
	std::vector<double> vox_times;
	std::vector<double> final_pass_times;
	std::vector<double> total_times;
    
	bool draw_light = false;

	while (window.ShouldContinue)
	{
#if ENABLE_UI
		// Start the Dear ImGui frame
		gui.StartFrame();
        
		ImGui::Begin("Model");
		ImGui::SliderAngle("X Rotation", &x_rotation_angle);
		ImGui::SliderAngle("Y Rotation", &y_rotation_angle);
		ImGui::SliderAngle("Z Rotation", &z_rotation_angle);
		ImGui::SliderFloat("Model Scale", &scale, 0, 1);
		ImGui::End();

		ImGui::Begin("Light Control");
		ImGui::SliderFloat3("Light Position", (float*)&rt_light.light_position, -20, 20);
		ImGui::ColorPicker3("Light Color", (float*)&rt_light.light_color);
		ImGui::Checkbox("Draw Light", &draw_light);
		ImGui::End();

		ImGui::Begin("Performance");
		//ImGui::PlotLines("Total Render Time",(float*)total_times.data(),1000);
		ImGui::End();
#endif

		{
			///NOTE(Tiago): The view matrix will only update if the camera parameters have changed
			view = cam.ViewMatrix();
            
			model = DirectX::XMMatrixRotationAxis({ 1.0f,0.0f,0.0f }, x_rotation_angle);
			model *= DirectX::XMMatrixRotationAxis({ 0.0f,1.0f,0.0f }, y_rotation_angle);
			model *= DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, z_rotation_angle);
			model *= DirectX::XMMatrixScaling(scale, scale, scale);
			model *= DirectX::XMMatrixTranslation(x_off, 0, 0);
			raster_cbufer.mvp = model * view * projection;
			raster_cbufer.vp = view * projection;
			raster_cbufer.voxel = voxelizer.voxel_space_conversion_matrix;
			light_buff.mvp = DirectX::XMMatrixTranslation(rt_light.light_position.x, rt_light.light_position.y, rt_light.light_position.z) * view * projection;
			light_buff.voxel = DirectX::XMMatrixScaling(1/0.01, 1/0.01, 1/0.01) * voxelizer.voxel_space_conversion_matrix;
			constant_buffer.UpdateData({ raster_cbufer });
			light_cbuffer.UpdateData({ light_buff });
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
        
		commandList.BindConstantBuffer(rtc_buffer, 6);
		commandList.BindConstantBuffer(constant_buffer, 1);
		light_map.BindUAV(commandList, 2);
		light_map.BindSRV(commandList, 5);
        
		timers[2].Start(commandList.GetRAWInterface(),0);
		sib_model.Draw(commandList, 3, 4);
		timers[2].Stop(commandList.GetRAWInterface(),0);
        
		//commandList.SendDrawCall();
        
		if(draw_light)
		{
		//Render Light
		{
			commandList.BindConstantBuffer(light_cbuffer, 1);
			commandList.BindVertexBuffer(*voxelizer.voxel_cube_vertex_buffer);
			commandList.BindIndexBuffer(*voxelizer.voxel_cube_index_buffer);
			commandList.SendDrawCall();
		}
		}

		commandList.BindVertexBuffer(vertex_buffer);
		commandList.BindIndexBuffer(index_buffer);
        
		commandList.BindConstantBuffer(constant_buffer, 1);
		commandList.BindTexture(texture, 3, 4);
        
		timers[1].Start(commandList.GetRAWInterface(),0);
		voxelizer.Voxelize(commandList, root_signature);
		timers[1].Stop(commandList.GetRAWInterface(),0);
        
		commandList->SetPipelineState(pso.GetPipelineStateObject());
		commandList.SetGraphicsRootSignature(root_signature);
        
		//voxelizer.BuildAccelerationStructure(device, commandList, fence);
        
		swapchain.SetViewport(commandList, swapchain.GetBackbufferResolution());
        
		rt_light.voxel_space_matrix = DirectX::XMMatrixScaling(1/initial_scale, 1/initial_scale, 1/initial_scale) * voxelizer.voxel_space_conversion_matrix;
		rtc_buffer.UpdateData({ rt_light });
		{
			commandList->SetPipelineState1(rtpso.GetRTPSO());
            
			//commandList.BindTLAS(voxelizer.acceleration_structure, 0);
			commandList.BindTLAS(tlas, 0);
			//rt_out.Bind(commandList, 2);
			light_map.Clear(commandList);
			light_map.BindComputeUAV(commandList, 2);
			commandList.BindComputeConstantBuffer(rtc_buffer, 1);
			voxelizer.normal_map.BindComputeUAV(commandList, 7);
            
            
			D3D12_DISPATCH_RAYS_DESC rays = {};
            
			rays.HitGroupTable.StartAddress = sbtable.GetHitGroupEntryAddress();
			rays.HitGroupTable.StrideInBytes = sbtable.GetHitGroupSectionSize();
			rays.HitGroupTable.SizeInBytes = sbtable.GetHitGroupEntrySize();
            
			rays.MissShaderTable.StartAddress = sbtable.GetMissEntryAddress();
			rays.MissShaderTable.StrideInBytes = sbtable.GetMissEntrySize();
			rays.MissShaderTable.SizeInBytes = sbtable.GetMissEntrySize();
            
			rays.RayGenerationShaderRecord.StartAddress = sbtable.GetRayGenEntryAddress();
			rays.RayGenerationShaderRecord.SizeInBytes = sbtable.GetRayGenEntrySize();
            
			rays.Width = 1;
			rays.Depth = 512;
			rays.Height = 512;
            
			timers[0].Start(commandList.GetRAWInterface(),0);
			commandList->DispatchRays(&rays);
			timers[0].Stop(commandList.GetRAWInterface(),0);
            
			//rt_out.CopyToBackbuffer(commandList,swapchain);
		}
        
		//voxelizer.Voxelize(cam);
#if ENABLE_UI
		gui.Render(commandList);
#endif
        
		swapchain.PrepareBackbufferForPresentation(commandList);
        
		for (unsigned int i = 0; i < 3; ++i)
		{
			timers[i].EndFrame(commandList.GetRAWInterface());
		}
        
		commandList.Close();
		device.GetGraphicsCommandQueue().ExecuteCommandList(commandList);
        
		swapchain.Present();
        
		device.GetGraphicsCommandQueue().Flush(fence);
        
		commandList.FullReset(pso);
        
        
		double rt_ms = timers[0].GetElapsedMS();
		double vox_ms = timers[1].GetElapsedMS();
		double final_pass_ms = timers[2].GetElapsedMS();
		double total_time = rt_ms + vox_ms + final_pass_ms;
        
		rt_times.push_back(rt_ms);
		vox_times.push_back(vox_ms);
		final_pass_times.push_back(final_pass_ms);
		total_times.push_back(total_time);
        
		for (unsigned int i = 0; i < 3; ++i)
		{
			timers[i].BeginFrame(commandList.GetRAWInterface());
		}
	}
    
	DeinitNVAPI();
    
	//average out everything
	{
		size_t sample_count = rt_times.size();;
		double average_rt = 0;
		for(auto ms:rt_times)
		{
			average_rt += ms;
		}
		average_rt /= sample_count;
        
		double average_vox = 0;
		for(auto ms:vox_times)
		{
			average_vox += ms;
		}
		average_vox /= sample_count;
        
		double average_final_pass = 0;
		for(auto ms:final_pass_times)
		{
			average_final_pass += ms;
		}
		average_final_pass /= sample_count;
        
		double average_total = 0;
		for(auto ms:total_times)
		{
			average_total += ms;
		}
		average_total /= sample_count;
        
		std::wofstream file(L"perf_vals.txt");
        file<<L"GPU:"<<device.name<<L"\n";
		file<<"Average Ray Tracing Dispatch Time (ms):"<<average_rt<<'\n';
		file<<"Average Voxelization Time (ms):"<<average_vox<<'\n';
		file<<"Average Final Pass Time (ms):"<<average_final_pass<<'\n';
		file<<"Average Total Time (ms):"<<average_total<<'\n';
		file<<"Sample Count:"<<sample_count<<'\n';
		file.close();
	}
}

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                            LPSTR lpCmdLine, int nCmdShow)
{
	DXR::Window window{ hInstance,nCmdShow,{1920, 1080},"DX Renderer" };
    
	std::thread main_dx12_thread(MainDirectXThread, std::ref(window));
    
	while (window.ShouldContinue)
	{
		window.UpdateWindow();
	}
    
	main_dx12_thread.join();
	return 0;
}