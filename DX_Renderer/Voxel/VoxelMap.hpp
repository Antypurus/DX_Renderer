#pragma once

#include <directxmath.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <cmath>

#include "../Core/Components/Resource/DescriptorHeap.hpp"

namespace DXR
{
    
	using namespace Microsoft::WRL;
	using namespace DirectX;
    
    struct Fence;
	struct GraphicsDevice;
	struct DescriptorHeap;
	struct GraphicsCommandList;
	struct Camera;
	struct RootSignature;
	struct PipelineStateObject;
	template<typename T> struct ConstantBuffer;
    
    static UINT Align(UINT Original, UINT AlignTo);
    
	struct Voxel_cbuffer
	{
		DirectX::XMMATRIX clip_space_matrix;
		DirectX::XMMATRIX voxel_space_matrix;
	};
    
    struct CPU_voxel_map
    {
        std::unique_ptr<BYTE[]> voxel_map_buffer = nullptr;
        UINT width = 0;
        UINT height = 0;
        UINT depth = 0;
        UINT row_pitch = 0;
        
        CPU_voxel_map() = default;
        CPU_voxel_map(const CPU_voxel_map& other);
        CPU_voxel_map(UINT width, UINT height, UINT depth, UINT row_pitch);
        CPU_voxel_map(float* data, UINT width, UINT height, UINT depth, UINT row_pitch);
        void operator=(const CPU_voxel_map& other);
        XMFLOAT4 Get(UINT x, UINT y, UINT z);
        void Update(float* data);
    };
    
    enum class MapType
    {
        R8Unorm         = DXGI_FORMAT_R8_UNORM,
        R8Uint          = DXGI_FORMAT_R8_UINT,
        R32Float        = DXGI_FORMAT_R32_FLOAT,
        R32Uint         = DXGI_FORMAT_R32_UINT,
        R8G8B8A8Unorm   = DXGI_FORMAT_R8G8B8A8_UNORM,
        R11G11B10Float  = DXGI_FORMAT_R11G11B10_FLOAT,
    };
    
	struct VoxelMap
	{
		ComPtr<ID3D12Resource> voxel_volume_texture = nullptr;
        ComPtr<ID3D12Resource> voxel_map_readback_buffer = nullptr;
		UINT width = 0;
		UINT height = 0;
		UINT depth = 0;
		DXGI_FORMAT format;
        UINT format_byte_size;
        
		UINT heap_index = 0xFF;
		DescriptorHeap* descriptor_heap = nullptr;
        
        UINT srv_heap_index;
        
        
        DescriptorHeap clear_heap;
        
		std::unique_ptr<ConstantBuffer<Voxel_cbuffer>> voxel_constant_buffer = nullptr;
        
        CPU_voxel_map cpu_buffer;
        
        VoxelMap() = default;
		VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth,MapType format, bool NeedsReadback = false);
		void BindUAV(GraphicsCommandList& command_list, UINT slot);
        void BindComputeUAV(GraphicsCommandList& command_list, UINT slot);
        void BindSRV(GraphicsCommandList& command_list, UINT slot);
        void Clear(GraphicsCommandList& command_list);
        CPU_voxel_map& ReadVoxelMap(GraphicsDevice& device ,GraphicsCommandList& command_list,Fence& fence);
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
        private:
        UINT DetermineFormatByteSize(MapType format);
        void CreateVoxelMap(GraphicsDevice& device);
        void CreateUAV(GraphicsDevice& device);
        void CreateSRV(GraphicsDevice& device);
        void CreateReadbackBuffer(GraphicsDevice& device);
	};
    
}
