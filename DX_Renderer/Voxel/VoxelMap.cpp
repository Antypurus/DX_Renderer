#include "VoxelMap.hpp"

#include "../Core/Components/GraphicsDevice.hpp"
#include "../Tooling/Validate.hpp"
#include "../Core/Components/Resource/HeapManager.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Camera/Camera.hpp"
#include "../Core/Components/Resource/GPU Buffers/ConstantBuffer.hpp"
#include "../Core/Components/Pipeline/PipelineStateObject.hpp"
#include "../Core/Components/Shader/Root Signature/RootSignature.hpp"
#include <vector>
#include <algorithm>

namespace DXR
{
	VoxelMap::VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth)
	{
		this->format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->CreateVoxelMap(device);
		this->CreateUAV(device);
		this->CreateVoxelConstantBuffer(device);
	}

	void VoxelMap::Bind(GraphicsCommandList& command_list, Camera& camera, RootSignature& root_signature, PipelineStateObject& pso, XMFLOAT3 AABB[2], XMMATRIX model)
	{

		this->SetViewport(command_list);
		this->CreateVoxelMatrix(camera, AABB, model);
		this->CreateClipMatrix(camera, AABB, model);
		this->UpdateVoxelConstantBuffer();

		command_list.SetGraphicsRootSignature(root_signature);
		command_list->SetPipelineState(pso.GetPipelineStateObject());

		command_list.BindConstantBuffer(*voxel_constant_buffer, 0);
		// Bind voxel map to slot 2
		command_list->SetGraphicsRootDescriptorTable(2, (*descriptor_heap).Get(heap_index));//TODO(Tiago): Make the slot not hardcoded
	}

	void VoxelMap::CreateVoxelMap(GraphicsDevice& device)
	{
		D3D12_RESOURCE_DESC resource_desc;
		resource_desc.Alignment = 0;
		resource_desc.Width = this->width;
		resource_desc.Height = this->height;
		resource_desc.DepthOrArraySize = this->depth;
		resource_desc.MipLevels = 1;//TODO(Tiago): How many? I dont really know how mip mapping works on the API level
		resource_desc.SampleDesc.Count = 1;
		resource_desc.SampleDesc.Quality = 0;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		resource_desc.Format = this->format;
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;

		DXCall(device->CreateCommittedResource(&heap_properties,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_COMMON,//TODO(Tiago): What is the best initial state?
			nullptr,
			IID_PPV_ARGS(&voxel_volume_texture)));
	}

	void VoxelMap::CreateUAV(GraphicsDevice& device)
	{
		this->heap_index = SRHeapManager::GetManager().Allocate();
		this->descriptor_heap = &SRHeapManager::GetManager().descriptor_heap;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		uav_desc.Format = this->format;
		uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		uav_desc.Texture3D.MipSlice = 0;
		uav_desc.Texture3D.FirstWSlice = 0;
		uav_desc.Texture3D.WSize = depth;

		device->CreateUnorderedAccessView(voxel_volume_texture.Get(),
			nullptr,
			&uav_desc,
			(*descriptor_heap)[heap_index]);
	}

	void VoxelMap::SetViewport(GraphicsCommandList& command_list)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		command_list->RSSetViewports(1, &viewport);
	}

	void VoxelMap::CreateVoxelMatrix(Camera& camera, XMFLOAT3 AABB[2], XMMATRIX model)
	{
		using namespace std;
		XMVECTOR extent_vector = XMVectorSubtract({ AABB[1].x,AABB[1].y,AABB[1].z,0 }, { AABB[0].x,AABB[0].y,AABB[0].z,0 });
		XMFLOAT3 extent;
		XMStoreFloat3(&extent, extent_vector);
		extent.x *= float(width + 2.0f) / float(width);
		extent.y *= float(height + 2.0f) / float(height);
		extent.z *= float(depth + 2.0f) / float(depth);
		extent.x = extent.y = extent.z = max(extent.x, max(extent.y, extent.z));

		XMVECTOR center_vector = XMVectorAdd({ AABB[1].x,AABB[1].y,AABB[1].z,0 }, { AABB[0].x,AABB[0].y,AABB[0].z,0 });
		center_vector = XMVectorScale(center_vector, 0.5f);
		XMFLOAT3 center;
		XMStoreFloat3(&center, center_vector);

		XMVECTOR voxel_space_vector = XMVectorScale({ extent.x,extent.y,extent.z,0 }, 0.5f);
		voxel_space_vector = XMVectorSubtract(center_vector, voxel_space_vector);
		XMFLOAT3 voxel_space;
		XMStoreFloat3(&voxel_space, voxel_space_vector);

		XMMATRIX voxel_matrix;
		XMMATRIX m1, m2;

		m1 = XMMatrixTranslation(voxel_space.x, voxel_space.y, voxel_space.z);
		m2 = XMMatrixScaling(width / extent.x, height / extent.y, depth / extent.z);

		voxel_matrix = XMMatrixMultiplyTranspose(m1, m2);

		voxel_space_matrix = model * camera.ViewMatrix() * voxel_matrix;
	}

	void VoxelMap::CreateClipMatrix(Camera& camera, XMFLOAT3 AABB[2], XMMATRIX model)
	{
		using namespace std;
		XMVECTOR extent_vector = XMVectorSubtract({ AABB[1].x,AABB[1].y,AABB[1].z,0 }, { AABB[0].x,AABB[0].y,AABB[0].z,0 });
		XMFLOAT3 extent;
		XMStoreFloat3(&extent, extent_vector);
		extent.x *= float(width + 2.0f) / float(width);
		extent.y *= float(height + 2.0f) / float(height);
		extent.z *= float(depth + 2.0f) / float(depth);
		extent.x = extent.y = extent.z = max(extent.x, max(extent.y, extent.z));

		XMVECTOR center_vector = XMVectorAdd({ AABB[1].x,AABB[1].y,AABB[1].z,0 }, { AABB[0].x,AABB[0].y,AABB[0].z,0 });
		center_vector = XMVectorScale(center_vector, 0.5f);
		XMFLOAT3 center;
		XMStoreFloat3(&center, center_vector);

		XMVECTOR voxel_space_vector = XMVectorScale({ extent.x,extent.y,extent.z,0 }, 0.5f);
		voxel_space_vector = XMVectorSubtract(center_vector, voxel_space_vector);
		XMFLOAT3 voxel_space;
		XMStoreFloat3(&voxel_space, voxel_space_vector);

		XMMATRIX voxel_projection_matrix;
		XMMATRIX m1, m2;

		m1 = XMMatrixTranslation(center.x, center.y, voxel_space.z);
		m2 = XMMatrixScaling(2 / extent.x, 2 / extent.y, 1 / extent.z);

		voxel_projection_matrix = XMMatrixMultiplyTranspose(m1, m2);

		clip_space_matrix = camera.ViewMatrix() * camera.ViewMatrix() * voxel_projection_matrix;
	}

	void VoxelMap::CreateVoxelConstantBuffer(GraphicsDevice& device)
	{
		Voxel_cbuffer cbuffer = {};
		cbuffer.clip_space_matrix = clip_space_matrix;
		cbuffer.voxel_space_matrix = voxel_space_matrix;

		voxel_constant_buffer = std::make_unique<ConstantBuffer<Voxel_cbuffer>>(device, std::vector({ cbuffer }));
		(*voxel_constant_buffer)->SetName(L"Voxel C Buffer");
	}

	void VoxelMap::UpdateVoxelConstantBuffer()
	{
		Voxel_cbuffer cbuffer = {};
		cbuffer.clip_space_matrix = clip_space_matrix;
		cbuffer.voxel_space_matrix = voxel_space_matrix;

		voxel_constant_buffer->UpdateData({ cbuffer });
	}

}