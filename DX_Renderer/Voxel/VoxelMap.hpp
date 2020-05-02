#pragma once

#include <directxmath.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

#include "../Core/Components/Resource/DescriptorHeap.hpp"

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
		ComPtr<ID3D12Resource> voxel_volume_texture = nullptr;
		UINT width = 0;
		UINT height = 0;
		UINT depth = 0;
		DXGI_FORMAT format;
        
		UINT heap_index = 0xFF;
		DescriptorHeap* descriptor_heap = nullptr;
        
		XMMATRIX voxel_space_matrix;
		XMMATRIX clip_space_matrix;
        
        DescriptorHeap clear_heap;
        
		std::unique_ptr<ConstantBuffer<Voxel_cbuffer>> voxel_constant_buffer = nullptr;
        
        VoxelMap() = default;
		VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth);
		void Bind(GraphicsCommandList& command_list, Camera& camera, RootSignature& root_signature, PipelineStateObject& pso, XMFLOAT3 AABB[2], XMMATRIX model = XMMatrixIdentity());
        private:
		void CreateVoxelMap(GraphicsDevice& device);
		void CreateUAV(GraphicsDevice& device);
		void SetViewport(GraphicsCommandList& command_list);
		void CreateVoxelMatrix(Camera& camera, XMFLOAT3 AABB[2], XMMATRIX model = XMMatrixIdentity());
		void CreateClipMatrix(Camera& camera, XMFLOAT3 AABB[2], XMMATRIX model = XMMatrixIdentity());
		void CreateVoxelConstantBuffer(GraphicsDevice& device);
		void UpdateVoxelConstantBuffer();
        void Clear(GraphicsCommandList& command_list);
	};
    
}
