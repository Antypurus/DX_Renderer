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
#include "Voxel/VXGI.hpp"
#include "Camera/Camera.hpp"
#include "Core/Components/Motion Estimation/MotionEstimation.hpp"
#include "Voxel/VoxelMap.hpp"

struct CBuffer
{
    DirectX::XMMATRIX mvp;
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
    
    //DXR::SceneVoxelizer voxelizer(device,vs,ps);
    
	DXR::RootSignature root_signature;
    
	DXR::DescriptorTableRootParameter uav_desc_table;
	uav_desc_table.AddUAVEntry(0);
    
	DXR::DescriptorTableRootParameter srv_desc_table;
	srv_desc_table.AddSRVEntry(0);
    
	DXR::DescriptorTableRootParameter sampler_desc_table;
	sampler_desc_table.AddSamplerEntry(0);
    
	root_signature.AddDescriptorTableRootParameter(uav_desc_table);
	root_signature.AddDescriptorTableRootParameter(srv_desc_table);
	root_signature.AddDescriptorTableRootParameter(sampler_desc_table);
    
	DXR::DescriptorRootParameter cbv_root_parameter(DXR::RootParameterDescriptorType::CBV, 0);
	root_signature.AddDescriptorRootParameter(cbv_root_parameter);
    
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
    
	DXR::Camera cam({ 0,0,-10 }, { 0,0,1 });
    
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, 1280.0f / 720.0f, 0.1f, 1000.0f);
	DirectX::XMMATRIX view = cam.ViewMatrix();
	DirectX::XMMATRIX model = DirectX::XMMatrixScaling(0.015f, 0.015f, 0.015f);
    
	DirectX::XMMATRIX mvp = model * view * projection;
    CBuffer raster_cbufer;
    raster_cbufer.mvp = mvp;
    DXR::ConstantBuffer<CBuffer> constant_buffer(device, {raster_cbufer});
    
	commandList.FullReset(pso);
    
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
	float x_rotation_angle = 0;
	float y_rotation_angle = 1;
	float z_rotation_angle = 0;
	float scale = 0.015f;
    
	auto sib_model = DXR::OBJModelLoader::Load("./DX_Renderer/Resources/Models/sponza/sponza.obj");
	auto vertex_buffer = sib_model.GenerateVertexBuffer(device, commandList);
	auto index_buffer = sib_model.GenerateIndexBuffer(device, commandList);
    
	DXR::GUI gui(device, window, swapchain);
    
	DXR::RayTracingOutput rt_out(device, commandList, swapchain);
    
	DXR::BLAS blas(device, commandList, vertex_buffer, index_buffer, true);
	DXR::TLAS tlas;
	tlas.AddInstance(blas, mvp, 0);
	tlas.BuildTLAS(device, commandList);
    
	DXR::RayGenSBTEntry raygen(rgs);
	raygen.AddResource(rt_out.GetGPUHandle());
	DXR::MissSBTEntry miss(ms);
	DXR::HitGroupSBTEntry hitgroup(L"HitGroup");
    
	DXR::ShaderBindingTable sbtable(device, rtpso, raygen, miss, hitgroup);
    
    DXR::MotionEstimator estimator(device);
    DXR::VoxelMap map(device,128,128,728);
    
	while (window.ShouldContinue)
	{
        //estimator.EstimateMotion(device,swapchain);
		// Start the Dear ImGui frame
		gui.StartFrame();
        
		ImGui::Begin("Window");
		ImGui::SliderAngle("X Rotation", &x_rotation_angle);
		ImGui::SliderAngle("Y Rotation", &y_rotation_angle);
		ImGui::SliderAngle("Z Rotation", &z_rotation_angle);
		ImGui::SliderFloat("Model Scale", &scale, 0, 1);
        
		ImGui::SliderFloat("Camera View Pitch", &cam.pitch, -90,90);
        ImGui::SliderFloat("Camera View Yaw", &cam.yaw, 0,360);
        
        cam.Rotate();
        
        ImGui::End();
        
        {
            ///NOTE(Tiago): The view matrix will only update if the camera parameters have changed
            view = cam.ViewMatrix();
            
            model = DirectX::XMMatrixRotationAxis({ 1.0f,0.0f,0.0f }, x_rotation_angle);
            model *= DirectX::XMMatrixRotationAxis({ 0.0f,1.0f,0.0f }, y_rotation_angle);
            model *= DirectX::XMMatrixRotationAxis({ 0.0f,0.0f,1.0f }, z_rotation_angle);
            model *= DirectX::XMMatrixScaling(scale, scale, scale);
            raster_cbufer.mvp = model * view * projection;
            constant_buffer.UpdateData({raster_cbufer});
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

        
        map.Bind(commandList,cam,root_signature,voxel_pso, sib_model.AABB, model);
        commandList.SendDrawCall();//NOTE(Tiago): Voxelization Draw Call

        swapchain.SetViewport(commandList,swapchain.GetBackbufferResolution());
        commandList->SetPipelineState(pso.GetPipelineStateObject());

        commandList.BindConstantBuffer(constant_buffer, 0);
        commandList.BindTexture(texture, 3, 4);
        
        commandList.SendDrawCall();

        swapchain.SetViewport(commandList,swapchain.GetBackbufferResolution());
        
        {
            commandList->SetPipelineState1(rtpso.GetRTPSO());
            
            commandList.BindTLAS(tlas, 1);
            rt_out.Bind(commandList, 2);
            
            D3D12_DISPATCH_RAYS_DESC rays = {};
            
            rays.HitGroupTable.StartAddress = sbtable.GetHitGroupEntryAddress();
            rays.HitGroupTable.StrideInBytes = sbtable.GetHitGroupSectionSize();
            rays.HitGroupTable.SizeInBytes = sbtable.GetHitGroupEntrySize();
            
            rays.MissShaderTable.StartAddress = sbtable.GetMissEntryAddress();
            rays.MissShaderTable.StrideInBytes = sbtable.GetMissEntrySize();
            rays.MissShaderTable.SizeInBytes = sbtable.GetMissEntrySize();
            
            rays.RayGenerationShaderRecord.StartAddress = sbtable.GetRayGenEntryAddress();
            rays.RayGenerationShaderRecord.SizeInBytes = sbtable.GetRayGenEntrySize();
            
            rays.Depth = 1;
            rays.Width = swapchain.GetBackbufferResolution().Width;
            rays.Height = swapchain.GetBackbufferResolution().Height;
            commandList->DispatchRays(&rays);
            
            //rt_out.CopyToBackbuffer(commandList,swapchain);
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
	DXR::Window window{ hInstance,nCmdShow,{1280,720},"DX Renderer" };
    
	std::thread main_dx12_thread(MainDirectXThread, std::ref(window));
    
	while (window.ShouldContinue)
	{
		window.UpdateWindow();
	}
    
	main_dx12_thread.join();
	return 0;
}