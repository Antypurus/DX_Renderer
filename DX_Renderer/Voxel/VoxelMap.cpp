#include "VoxelMap.hpp"

#include "../Core/Components/GraphicsDevice.hpp"
#include "../Tooling/Validate.hpp"

namespace DXR
{
    VoxelMap::VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth)
    {
        this->width = width;
        this->height = height;
        this->depth = depth;
        this->CreateVoxelMap(device);
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
        resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        resource_desc.Format = DXGI_FORMAT_R32_UINT;
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        
        D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
        
        DXCall(device->CreateCommittedResource(&heap_properties,
                                       D3D12_HEAP_FLAG_NONE,//TODO(Tiago): Allow UAV Access
                                       &resource_desc,
                                       D3D12_RESOURCE_STATE_COMMON,//TODO(Tiago): What is the best initial state?
                                       nullptr,
                                       IID_PPV_ARGS(&voxel_volume_texture)));
    }
    
}
