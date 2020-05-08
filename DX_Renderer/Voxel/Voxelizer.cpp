#define _USE_MATH_DEFINES

#include "Voxelizer.hpp"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Camera/Camera.hpp"
#include "../Core/Components/Swapchain.hpp"

#include <cmath>
#include <string>

namespace DXR
{
    
	Voxelizer::Voxelizer(GraphicsDevice& device,
                         GraphicsCommandList& command_list,
                         RootSignature& root_signature,
                         Model& model,
                         XMMATRIX mvp)
	{
		this->CreateVoxelCube(device, command_list);
		this->model_vertex_buffer = model.GenerateVertexBuffer(device, command_list);
		this->model_index_buffer = model.GenerateIndexBuffer(device, command_list);
		this->CreateVoxelizationShaders();
		this->voxel_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH);
		this->model = &model;
		this->pso = PipelineStateObject(device,
                                        this->voxelization_vertex_shader.GetShaderBytecode(),
                                        this->voxelization_pixel_shader.GetShaderBytecode(),
                                        root_signature,
                                        OBJVertex::GetInputLayout(),
                                        Swapchain::m_backbuffer_format,
                                        DepthStencilBuffer::DepthStencilBufferFormat);
		//NOTE(Tiago): dummy/placeholder data is required to create the constant buffer while the matrices are not yet computed
		std::vector<Voxelization_CBuffer> intermediate;
		intermediate.push_back({ XMMatrixIdentity(), XMMatrixIdentity() });
		this->voxelization_cbuffer_x = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device, intermediate);
        this->voxelization_cbuffer_y = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device, intermediate);
        this->voxelization_cbuffer_z = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device, intermediate);
        this->voxelization_cbuffer_x->GetResource()->SetName(L"X Axis Voxelization CBuffer");
        this->voxelization_cbuffer_y->GetResource()->SetName(L"Y Axis Voxelization CBuffer");
        this->voxelization_cbuffer_z->GetResource()->SetName(L"Z Axis Voxelization CBuffer");
		this->CalculateVoxelizationSupportData();
	}
    
	void Voxelizer::Voxelize(GraphicsCommandList& command_list, Camera& camera, RootSignature& root_signature, XMMATRIX model_matrix, UINT constant_buffer_slot, UINT voxel_map_uav_slot)
	{
        auto scale = XMMatrixScaling(VOXEL_SCALE,VOXEL_SCALE,VOXEL_SCALE);
        //X-Axis View
		//voxel_map.Clear(command_list);
		this->SetViewport(command_list);
		this->UpdateVoxelizationMatrices(camera, scale * XMMatrixIdentity());
		this->UpdateVoxelizationCBufferX();
		command_list.SetGraphicsRootSignature(root_signature);
		command_list->SetPipelineState(pso.GetPipelineStateObject());
		command_list.BindConstantBuffer(*voxelization_cbuffer_x, constant_buffer_slot);
		this->voxel_map.BindUAV(command_list, voxel_map_uav_slot);
		command_list.BindVertexBuffer(model_vertex_buffer);
		command_list.BindIndexBuffer(model_index_buffer);
		command_list.SendDrawCall();
        //Y-Axis View
		this->SetViewport(command_list);
		this->UpdateVoxelizationMatrices(camera, scale * XMMatrixRotationAxis({0.0f,1.0f,0.0f},0.5f * M_PI));
		this->UpdateVoxelizationCBufferY();
		command_list.SetGraphicsRootSignature(root_signature);
		command_list->SetPipelineState(pso.GetPipelineStateObject());
		command_list.BindConstantBuffer(*voxelization_cbuffer_y, constant_buffer_slot);
		//this->voxel_map.BindUAV(command_list, voxel_map_uav_slot);
		command_list.BindVertexBuffer(model_vertex_buffer);
		command_list.BindIndexBuffer(model_index_buffer);
		command_list.SendDrawCall();
        //Z-Axis View
		this->SetViewport(command_list);
		this->UpdateVoxelizationMatrices(camera, scale * XMMatrixRotationAxis({1.0f,0.0f,0.0f},0.5f * M_PI));
		this->UpdateVoxelizationCBufferZ();
		command_list.SetGraphicsRootSignature(root_signature);
		command_list->SetPipelineState(pso.GetPipelineStateObject());
		command_list.BindConstantBuffer(*voxelization_cbuffer_z, constant_buffer_slot);
		//this->voxel_map.BindUAV(command_list, voxel_map_uav_slot);
		command_list.BindVertexBuffer(model_vertex_buffer);
		command_list.BindIndexBuffer(model_index_buffer);
		command_list.SendDrawCall();
	}
    
	void Voxelizer::CalculateVoxelizationSupportData()
	{
		using namespace std;
        
		XMFLOAT3* AABB = model->AABB;
		XMVECTOR extent_vector = XMVectorSubtract({ AABB[1].x,AABB[1].y,AABB[1].z,1 }, { AABB[0].x,AABB[0].y,AABB[0].z,1 });
		XMStoreFloat3(&extent, extent_vector);
		extent.x *= float(VOXEL_WIDTH + 2.0f) / float(VOXEL_WIDTH);
		extent.y *= float(VOXEL_HEIGHT + 2.0f) / float(VOXEL_HEIGHT);
		extent.z *= float(VOXEL_DEPTH + 2.0f) / float(VOXEL_DEPTH);
		//extent.x = extent.y = extent.z = max(extent.x, max(extent.y, extent.z));
        
		XMVECTOR center_vector = XMVectorAdd({ AABB[1].x,AABB[1].y,AABB[1].z,1 }, { AABB[0].x,AABB[0].y,AABB[0].z,1 });
		center_vector = XMVectorScale(center_vector, 0.5f);
		XMStoreFloat3(&center, center_vector);
        
		XMVECTOR voxel_space_vector = XMVectorScale({ extent.x,extent.y,extent.z,1 }, 0.5f);
		voxel_space_vector = XMVectorSubtract(center_vector, voxel_space_vector);
		XMStoreFloat3(&voxel_space, voxel_space_vector);
	}
    
	void Voxelizer::UpdateVoxelizationMatrices(Camera& camera, XMMATRIX& model_matrix)
	{
		XMMATRIX voxel_matrix;
		XMMATRIX m1, m2;
		m1 = XMMatrixTranslation(-voxel_space.x, -voxel_space.y, -voxel_space.z);
		m2 = XMMatrixScaling(VOXEL_WIDTH / extent.x, VOXEL_HEIGHT / extent.y, VOXEL_DEPTH / extent.z);
		voxel_matrix = m1 * m2;
		voxel_space_conversion_matrix = voxel_matrix;
        
		XMMATRIX clip_matrix;
		m1 = XMMatrixTranslation(-center.x, -center.y, -voxel_space.z);
		m2 = XMMatrixScaling(2.0f / extent.x, 2.0f / extent.y, 1.0f / extent.z);
		clip_matrix = m1 * m2;
		clip_space_conversion_matrix = model_matrix * clip_matrix;
	}
    
	void Voxelizer::UpdateVoxelizationCBufferX()
	{
		voxelization_cbuffer_x->UpdateData({ {
                                                   clip_space_conversion_matrix,
                                                   voxel_space_conversion_matrix
                                               } });
	}
    
    void Voxelizer::UpdateVoxelizationCBufferY()
	{
		voxelization_cbuffer_y->UpdateData({ {
                                                   clip_space_conversion_matrix,
                                                   voxel_space_conversion_matrix
                                               } });
	}
    
    void Voxelizer::UpdateVoxelizationCBufferZ()
	{
		voxelization_cbuffer_z->UpdateData({ {
                                                   clip_space_conversion_matrix,
                                                   voxel_space_conversion_matrix
                                               } });
	}
    
	void Voxelizer::CreateVoxelizationShaders()
	{
		const std::wstring voxel_shader_path = L"./DX_Renderer/Resources/Shaders/Voxelization.hlsl";
		const std::wstring VS_entrypoint = L"VoxelVSMain";
		const std::wstring PS_entrypoint = L"VoxelPSMain";
		this->voxelization_vertex_shader = VertexShader::CompileShaderFromFile(voxel_shader_path, VS_entrypoint);
		this->voxelization_pixel_shader = PixelShader::CompileShaderFromFile(voxel_shader_path, PS_entrypoint);
	}
    
    
	void Voxelizer::SetViewport(GraphicsCommandList& command_list)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.Width = (FLOAT)VOXEL_WIDTH;
		viewport.Height = (FLOAT)VOXEL_HEIGHT;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		command_list->RSSetViewports(1, &viewport);
	}
    
	void Voxelizer::CreateVoxelCube(GraphicsDevice& device, GraphicsCommandList& command_list)
	{
		std::vector<Vertex> vertices = {
			{{-0.5,  0.5, -0.5},{0,0}},
			{{ 0.5,  0.5, -0.5},{0,0}},
			{{ 0.5,  0.5,  0.5},{0,0}},
			{{ 0.5, -0.5, -0.5},{0,0}},
			{{ 0.5, -0.5,  0.5},{0,0}},
			{{-0.5, -0.5, -0.5},{0,0}},
			{{-0.5, -0.5,  0.5},{0,0}},
			{{-0.5,  0.5,  0.5},{0,0}}
		};
		std::vector<UINT> indices = {
			6,4,2,
			6,2,7,
			4,3,1,
			4,1,2,
			7,2,1,
			7,1,0,
			5,6,0,
			6,7,0,
			5,0,1,
			5,1,3,
			5,3,6,
			3,4,6
		};
		voxel_cube_vertex_buffer = std::make_unique<VertexBuffer<Vertex>>(device, command_list, vertices);
		voxel_cube_index_buffer = std::make_unique<IndexBuffer>(device, command_list, indices);
		voxel_cube_blas = std::make_unique<BLAS>(device, command_list, *voxel_cube_vertex_buffer, *voxel_cube_index_buffer);
	}
    
	void Voxelizer::BuildAccelerationStructure(GraphicsDevice& device, GraphicsCommandList& command_list, Fence& fence)
	{
		if (acceleration_structure.GetInstanceCount() == 0)
		{
			CPU_voxel_map& buffer = voxel_map.ReadVoxelMap(device, command_list, fence);
			for (size_t x = 0; x < VOXEL_WIDTH; ++x)
			{
				for (size_t y = 0; y < VOXEL_HEIGHT; ++y)
				{
					for (size_t z = 0; z < VOXEL_DEPTH; ++z)
					{
						XMFLOAT4 color = buffer.Get(x, y, z);
						if (color.w != 0)
						{
							acceleration_structure.AddInstance(*voxel_cube_blas, XMMatrixTranspose(XMMatrixScaling(1/VOXEL_SCALE,1/VOXEL_SCALE,1/VOXEL_SCALE) *
                                                                                                   XMMatrixTranslation(x, y, z)), 0);
						}
					}
				}
			}
			if (acceleration_structure.GetInstanceCount() != 0)
			{
				acceleration_structure.BuildTLAS(device, command_list);
			}
		}
	}
    
}
