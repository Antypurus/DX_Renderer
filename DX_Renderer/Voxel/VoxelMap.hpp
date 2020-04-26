#pragma once

#include <directxmath.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

namespace DXR
{
    
    using namespace Microsoft::WRL;
    using namespace DirectX;
    
    struct GraphicsDevice;
    struct DescriptorHeap;
    struct GraphicsCommandList;
    struct Camera;
    struct RootSignature;
    struct PipelineStateObject;
    template<typename T> struct ConstantBuffer;
    
    struct Voxel_cbuffer
    {
        DirectX::XMMATRIX clip_space_matrix;
        DirectX::XMMATRIX voxel_space_matrix;
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
        
        std::unique_ptr<ConstantBuffer<Voxel_cbuffer>> voxel_constant_buffer;
        
        VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth);
        void Bind(GraphicsCommandList& command_list, Camera& camera, RootSignature& root_signature, PipelineStateObject& pso, XMMATRIX model = XMMatrixIdentity());
        private:
        void CreateVoxelMap(GraphicsDevice& device);
        void CreateUAV(GraphicsDevice& device);
        void SetViewport(GraphicsCommandList& command_list);
        void CreateVoxelMatrix(Camera& camera, XMMATRIX model = XMMatrixIdentity());
        void CreateClipMatrix(Camera& camera, XMMATRIX model = XMMatrixIdentity());
        void CreateVoxelConstantBuffer(GraphicsDevice& device);
        void UpdateVoxelConstantBuffer();
    };
    
}
