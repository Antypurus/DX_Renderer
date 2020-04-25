#pragma once

#include <directxmath.h>
#include <d3d12.h>
#include <wrl.h>

namespace DXR
{
    
    using namespace Microsoft::WRL;
    using namespace DirectX;
    
    struct GraphicsDevice;
    struct DescriptorHeap;
    struct GraphicsCommandList;
    struct Camera;
    
    struct Voxel_cbuffer
    {
        DirectX::XMMATRIX clip_space_matrix;
        DirectX::XMMATRIX voxel_space;
    };
    
    struct VoxelMap
    {
        ComPtr<ID3D12Resource> voxel_volume_texture;
        UINT width;
        UINT height;
        UINT depth;
        DXGI_FORMAT format;
        
        UINT heap_index;
        DescriptorHeap* descriptor_heap;
        
        XMMATRIX voxel_space_matrix;
        XMMATRIX clip_space_matrix;
        
        VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth);
        void Bind(GraphicsCommandList& command_list, Camera& camera);
        private:
        void CreateVoxelMap(GraphicsDevice& device);
        void CreateUAV(GraphicsDevice& device);
        void SetViewport(GraphicsCommandList& command_list);
        void CreateVoxelMatrix();
    };
    
}
