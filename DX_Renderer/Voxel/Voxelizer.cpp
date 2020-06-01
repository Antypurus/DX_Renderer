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
        this->CreateVoxelizationRootSignature(device);
		this->CreateVoxelCube(device, command_list);
		this->model_vertex_buffer = model.GenerateVertexBuffer(device, command_list);
		this->model_index_buffer = model.GenerateIndexBuffer(device, command_list);
		this->CreateVoxelizationShaders();
		this->CreateVoxelMaps(device);
		this->model = &model;
		this->pso = PipelineStateObject(device,
                                        this->voxelization_vertex_shader.GetShaderBytecode(),
                                        this->voxelization_pixel_shader.GetShaderBytecode(),
                                        voxelization_root_signature,
                                        OBJVertex::GetInputLayout(),
                                        Swapchain::m_backbuffer_format,
                                        DepthStencilBuffer::DepthStencilBufferFormat);
		this->CreateVoxelizationConstantBuffers(device,command_list);
		this->CalculateVoxelizationSupportData();
	}
    
    void Voxelizer::CreateVoxelMaps(GraphicsDevice& device)
    {
        this->albedo_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, MapType::R8G8B8A8Unorm,MapType::R32Uint);
        this->albedo_map.voxel_volume_texture->SetName(L"Albedo Map");
        
        this->ocupancy_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, MapType::R8Uint,true);
        this->ocupancy_map.voxel_volume_texture->SetName(L"Ocupancy Map");
        
        this->diffuse_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, MapType::R8G8B8A8Unorm,MapType::R32Uint);
        this->diffuse_map.voxel_volume_texture->SetName(L"Diffuse Map");
        
        this->specular_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, MapType::R8G8B8A8Unorm,MapType::R32Uint);
        this->specular_map.voxel_volume_texture->SetName(L"Specular Map");
        
        this->exponent_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, MapType::R32Float);
        this->exponent_map.voxel_volume_texture->SetName(L"Specular Exponent Map");
        
        this->normal_map = VoxelMap(device, VOXEL_WIDTH, VOXEL_HEIGHT, VOXEL_DEPTH, MapType::R8G8B8A8Unorm,MapType::R32Uint);
        this->normal_map.voxel_volume_texture->SetName(L"Normal Map");
    }
    
	void Voxelizer::Voxelize(GraphicsCommandList& command_list, RootSignature& root_signature)
	{
		albedo_map.Clear(command_list);
        ocupancy_map.Clear(command_list);
        diffuse_map.Clear(command_list);
        specular_map.Clear(command_list);
        exponent_map.Clear(command_list);
        normal_map.Clear(command_list);
		this->SetViewport(command_list);
		command_list.SetGraphicsRootSignature(voxelization_root_signature);
		command_list->SetPipelineState(pso.GetPipelineStateObject());
		this->albedo_map.BindUAV(command_list, 1);
        this->ocupancy_map.BindUAV(command_list, 5);
        this->diffuse_map.BindUAV(command_list, 6);
        this->specular_map.BindUAV(command_list, 7);
        this->exponent_map.BindUAV(command_list, 8);
        this->normal_map.BindUAV(command_list, 9);
        command_list.BindVertexBuffer(model_vertex_buffer);
		// Per Submesh Component
        for(auto& submesh:model->submeshes)
        {
            this->VoxelizeSubmesh(submesh, command_list);
        }
        command_list.SetGraphicsRootSignature(root_signature);
	}
    
    void Voxelizer::VoxelizeSubmesh(Submesh& submesh, GraphicsCommandList& command_list)
    {
        if(submesh.material->has_texture)
        {
            command_list.BindTexture(*submesh.material->texture,2,3);
            command_list.BindIndexBuffer(*submesh.index_buffer);
            command_list.BindConstantBuffer(*submesh.material->material_cbuffer,4);
            //Z-Axis View
            this->ZAxisVoxelizationCall(command_list, 0);
            //X-Axis View
            this->XAxisVoxelizationCall(command_list, 0);
            //Y-Axis View
            this->YAxisVoxelizationCall(command_list, 0);
        }
    }
    
    void Voxelizer::XAxisVoxelizationCall(GraphicsCommandList& command_list, UINT constant_buffer_slot)
    {
        this->UpdateVoxelizationMatrices(XMMatrixRotationAxis({0.0f,1.0f,0.0f},0.5f * M_PI));
		this->UpdateVoxelizationCBufferX();
        command_list.BindConstantBuffer(*voxelization_cbuffer_x, constant_buffer_slot);
        command_list.SendDrawCall();
    }
    
    void Voxelizer::YAxisVoxelizationCall(GraphicsCommandList& command_list, UINT constant_buffer_slot)
    {
        this->UpdateVoxelizationMatrices(XMMatrixRotationAxis({1.0f,0.0f,0.0f},0.5f * M_PI));
		this->UpdateVoxelizationCBufferY();
        command_list.BindConstantBuffer(*voxelization_cbuffer_y, constant_buffer_slot);
        command_list.SendDrawCall();
    }
    
    void Voxelizer::ZAxisVoxelizationCall(GraphicsCommandList& command_list, UINT constant_buffer_slot)
    {
        this->UpdateVoxelizationMatrices(XMMatrixIdentity());
        this->UpdateVoxelizationCBufferZ();
        command_list.BindConstantBuffer(*voxelization_cbuffer_z, constant_buffer_slot);
        command_list.SendDrawCall();
    }
    
    void Voxelizer::CreateVoxelizationRootSignature(GraphicsDevice& device)
    {
        DXR::DescriptorTableRootParameter cbv_desc_table;
        cbv_desc_table.AddCBVEntry(0);
        
        DXR::DescriptorTableRootParameter albedo_map_desc_table;
        albedo_map_desc_table.AddUAVEntry(0);
        
        DXR::DescriptorTableRootParameter srv_desc_table;
        srv_desc_table.AddSRVEntry(0);
        
        DXR::DescriptorTableRootParameter sampler_desc_table;
        sampler_desc_table.AddSamplerEntry(0);
        
        voxelization_root_signature.AddDescriptorTableRootParameter(cbv_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(albedo_map_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(srv_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(sampler_desc_table);
        
        DXR::DescriptorTableRootParameter material_props_desc_table;
        material_props_desc_table.AddCBVEntry(1);
        
        DXR::DescriptorTableRootParameter ocupancy_map_desc_table;
        ocupancy_map_desc_table.AddUAVEntry(1);
        
        DXR::DescriptorTableRootParameter diffuse_map_desc_table;
        diffuse_map_desc_table.AddUAVEntry(2);
        
        DXR::DescriptorTableRootParameter specular_map_desc_table;
        specular_map_desc_table.AddUAVEntry(3);
        
        DXR::DescriptorTableRootParameter exponent_map_desc_table;
        exponent_map_desc_table.AddUAVEntry(4);
        
        DXR::DescriptorTableRootParameter normal_map_desc_table;
        normal_map_desc_table.AddUAVEntry(5);
        
        voxelization_root_signature.AddDescriptorTableRootParameter(material_props_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(ocupancy_map_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(diffuse_map_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(specular_map_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(exponent_map_desc_table);
        voxelization_root_signature.AddDescriptorTableRootParameter(normal_map_desc_table);
        
        voxelization_root_signature.CreateRootSignature(device);
    }
    
    void Voxelizer::CreateVoxelizationConstantBuffers(GraphicsDevice& device, GraphicsCommandList& command_list)
    {
        //NOTE(Tiago): dummy/placeholder data is required to create the constant buffer while the matrices are not yet computed
		std::vector<Voxelization_CBuffer> intermediate;
		intermediate.push_back({ XMMatrixIdentity(), XMMatrixIdentity() });
		this->voxelization_cbuffer_x = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device, intermediate);
        this->voxelization_cbuffer_y = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device, intermediate);
        this->voxelization_cbuffer_z = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device, intermediate);
        this->voxelization_cbuffer_x->GetResource()->SetName(L"X Axis Voxelization CBuffer");
        this->voxelization_cbuffer_y->GetResource()->SetName(L"Y Axis Voxelization CBuffer");
        this->voxelization_cbuffer_z->GetResource()->SetName(L"Z Axis Voxelization CBuffer");
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
		extent.x = extent.y = extent.z = max(extent.x, max(extent.y, extent.z));
        
		XMVECTOR center_vector = XMVectorAdd({ AABB[1].x,AABB[1].y,AABB[1].z,1 }, { AABB[0].x,AABB[0].y,AABB[0].z,1 });
		center_vector = XMVectorScale(center_vector, 0.5f);
		XMStoreFloat3(&center, center_vector);
        
		XMVECTOR voxel_space_vector = XMVectorScale({ extent.x,extent.y,extent.z,1 }, 0.5f);
		voxel_space_vector = XMVectorSubtract(center_vector, voxel_space_vector);
		XMStoreFloat3(&voxel_space, voxel_space_vector);
	}
    
	void Voxelizer::UpdateVoxelizationMatrices(XMMATRIX& model_matrix)
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
        
        const std::wstring no_tex_voxel_shader_path = L"./DX_Renderer/Resources/Shaders/no_tex_Voxelization.hlsl";
		this->no_tex_voxelization_vertex_shader = VertexShader::CompileShaderFromFile(no_tex_voxel_shader_path, VS_entrypoint);
		this->no_tex_voxelization_pixel_shader = PixelShader::CompileShaderFromFile(no_tex_voxel_shader_path, PS_entrypoint);
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
			CPU_voxel_map& buffer = albedo_map.ReadVoxelMap(device, command_list, fence);//TODO(Tiago): Swap this to read from the ocupancy map, but since this part is kind of broken right not it doesnt really matter that much, we can kind even remove the creation of the acceleration structure.
			for (size_t x = 0; x < VOXEL_WIDTH; ++x)
			{
				for (size_t y = 0; y < VOXEL_HEIGHT; ++y)
				{
					for (size_t z = 0; z < VOXEL_DEPTH; ++z)
					{
						XMFLOAT4 color = buffer.Get(x, y, z);
						if (color.x != 0.0f||color.y != 0.0f||color.z != 0.0f||color.w != 0.0f)
						{
							acceleration_structure.AddInstance(*voxel_cube_blas, XMMatrixTranspose(XMMatrixTranslation(x, y, z)), 0);
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
