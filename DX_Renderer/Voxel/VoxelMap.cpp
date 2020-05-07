#include "VoxelMap.hpp"

#include "../Core/Components/GraphicsDevice.hpp"
#include "../Tooling/Validate.hpp"
#include "../Core/Components/Resource/HeapManager.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Camera/Camera.hpp"
#include "../Core/Components/Resource/GPU Buffers/ConstantBuffer.hpp"
#include "../Core/Components/Pipeline/PipelineStateObject.hpp"
#include "../Core/Components/Shader/Root Signature/RootSignature.hpp"
#include "../Core/Components/Fence.hpp"
#include "../Core/Components/Resource/ResourceBarrier.hpp"
#include <vector>
#include <algorithm>

namespace DXR
{
    UINT Align(UINT Original, UINT AlignTo)
    {
		return std::lround(std::ceil(Original / (double)AlignTo)) * AlignTo;
	}
    
    CPU_voxel_map::CPU_voxel_map(float* data, UINT width, UINT height, UINT depth, UINT row_pitch)
        :width(width),height(height),depth(depth),row_pitch(row_pitch)
    {
        voxel_map_buffer = std::make_unique<BYTE[]>(row_pitch * height * depth);
        memcpy((void*)voxel_map_buffer.get(), data, row_pitch * height * depth);
    }
    
    XMFLOAT4 CPU_voxel_map::Get(UINT x, UINT y, UINT z)
    {
        float red = 0;
        float green = 0;
        float blue = 0;
        float alpha = 0;
        
        float* base = (float*)&voxel_map_buffer[z * (height * row_pitch) + y * row_pitch + x * 4*4];
        red = base[0];
        green = base[1];
        blue = base[2];
        alpha = base[3];
        return {red, green, blue, alpha};
    }
    
	VoxelMap::VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth)
	{
		this->format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        this->format_byte_size = 4*4;
		this->width = width;
		this->height = height;
		this->depth = depth;
        
		clear_heap = device.CreateConstantBufferDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		this->CreateVoxelMap(device);
		this->CreateReadbackBuffer(device);
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
    
	void VoxelMap::CreateReadbackBuffer(GraphicsDevice& device)
	{
		D3D12_RESOURCE_DESC buffer_desc = {};
		buffer_desc.SampleDesc.Count = 1;
		buffer_desc.SampleDesc.Quality = 0;
		buffer_desc.Alignment = 0;
		buffer_desc.MipLevels = 1;
		buffer_desc.DepthOrArraySize = 1;
		buffer_desc.Width = Align(width * format_byte_size, 256) * height * depth;
		buffer_desc.Height = 1;
		buffer_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buffer_desc.Format = DXGI_FORMAT_UNKNOWN;
		buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        
		D3D12_HEAP_PROPERTIES heap_desc = {};
		heap_desc.Type = D3D12_HEAP_TYPE_READBACK;
		heap_desc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_desc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_desc.CreationNodeMask = 1;
		heap_desc.VisibleNodeMask = 1;
        
		DXCall(device->CreateCommittedResource(&heap_desc,
                                               D3D12_HEAP_FLAG_NONE,
                                               &buffer_desc,
                                               D3D12_RESOURCE_STATE_COPY_DEST,
                                               nullptr,
                                               IID_PPV_ARGS(&voxel_map_readback_buffer)));
		voxel_map_readback_buffer->SetName(L"Voxel Map Readback buffer");
	}
    
    CPU_voxel_map VoxelMap::ReadVoxelMap(GraphicsDevice& device, GraphicsCommandList& command_list, Fence& fence)
	{
		TransitionResourceBarrier barrier1(*voxel_volume_texture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		barrier1.ExecuteResourceBarrier(command_list);
        
		D3D12_TEXTURE_COPY_LOCATION destination = {};
		destination.pResource = voxel_map_readback_buffer.Get();
		destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		destination.PlacedFootprint.Offset = 0;
		destination.PlacedFootprint.Footprint.Format = format;
		destination.PlacedFootprint.Footprint.Width = width;
		destination.PlacedFootprint.Footprint.Height = height;
		destination.PlacedFootprint.Footprint.Depth = depth;
		destination.PlacedFootprint.Footprint.RowPitch = Align(width * format_byte_size, 256);
        
		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.pResource = voxel_volume_texture.Get();
		source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		source.SubresourceIndex = 0;
        
        D3D12_BOX source_box = {};
        source_box.left = 0;
        source_box.top = 0;
        source_box.front = 0;
        source_box.right = width;
        source_box.bottom = height;
        source_box.back = depth;
        
		D3D12_RANGE read_range = {};
		read_range.Begin = 0;
		read_range.End = width * height * depth * format_byte_size -1;
        
		command_list->CopyTextureRegion(&destination, 0, 0, 0, &source, &source_box);
        
		device.GetGraphicsCommandQueue().Flush(fence);
		FLOAT* data = nullptr;
		voxel_map_readback_buffer->Map(0, &read_range, (void**)&data);
        
        //TODO(Tiago): Update an already existing voxel map instead of creating a new one? seems like it would be less stressfull if it had to be done every frame
        CPU_voxel_map ret = {data, width, height, depth, Align(width * format_byte_size,256)};
        
        voxel_map_readback_buffer->Unmap(0, nullptr);
        
        TransitionResourceBarrier barrier2(*voxel_volume_texture.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        barrier2.ExecuteResourceBarrier(command_list);
        
        return ret;
    }
    
}