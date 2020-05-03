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

		clear_heap = device.CreateConstantBufferDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		this->CreateVoxelMap(device);
		this->CreateUAV(device);
	}

	void VoxelMap::BindUAV(GraphicsCommandList& command_list, UINT slot)
	{
		// Bind voxel map to slot 2
		command_list->SetGraphicsRootDescriptorTable(slot, (*descriptor_heap).Get(heap_index));//TODO(Tiago): Make the slot not hardcoded
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
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,//TODO(Tiago): What is the best initial state?
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
		device->CreateUnorderedAccessView(voxel_volume_texture.Get(),
			nullptr,
			&uav_desc,
			clear_heap[0]);
	}

	void VoxelMap::Clear(GraphicsCommandList& command_list)
	{
		const FLOAT clear[4] = { 0,0,0,0 };
		command_list->ClearUnorderedAccessViewFloat(clear_heap.Get(0),
			clear_heap[0],
			voxel_volume_texture.Get(),
			clear,
			0, NULL);
	}

}