#include "VoxelMap.hpp"

#include "../Core/Components/GraphicsDevice.hpp"

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
    }
    
}
