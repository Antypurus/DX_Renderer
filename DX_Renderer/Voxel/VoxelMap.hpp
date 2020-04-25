#pragma once

#include<d3d12.h>
#include<wrl.h>

namespace DXR
{
    
    using namespace Microsoft::WRL;
    
    struct GraphicsDevice;
    struct DescriptorHeap;
    
    struct VoxelMap
    {
        ComPtr<ID3D12Resource> voxel_volume_texture;
        UINT width;
        UINT height;
        UINT depth;
        DXGI_FORMAT format;
        
        UINT heap_index;
        DescriptorHeap* descriptor_heap;
        
        VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth);
        private:
        void CreateVoxelMap(GraphicsDevice& device);
        void CreateUAV(GraphicsDevice& device);
    };
    
}
