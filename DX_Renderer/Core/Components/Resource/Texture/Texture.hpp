#pragma once
#include <d3d12.h>

#include "../../../Windows Abstractions/TextureFS.hpp"
#include "../DescriptorHeap.hpp"
#include "TextureUploadBuffer.hpp"
#include "Sampler.h"

namespace  DXR
{
	struct GraphicsCommandList;
	struct GraphicsDevice;
	struct GPUDefaultBuffer;
	struct GPUUploadBuffer;
	//struct TextureUploadBuffer;

	struct Texture: public Resource
	{
	public:
		friend GraphicsCommandList;
	private:
		Sampler m_sampler;
		TextureData m_texture_data;
		DXGI_FORMAT m_texture_format;
		D3D12_RESOURCE_DESC m_resource_description = {};
		D3D12_SHADER_RESOURCE_VIEW_DESC m_srv_desc = {};
		DescriptorHeap m_heap;
		std::unique_ptr<TextureUploadBuffer> m_upload_buffer = nullptr;
		std::unique_ptr<GPUDefaultBuffer> m_texture_buffer = nullptr;
	public:
		Texture(const std::wstring& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList);
		DescriptorHeap* GetSRVHeap();
		DescriptorHeap* GetSamplerHeap();
	private:
		void CreateShaderResourceViewDescription(GraphicsDevice& Device) const;
		void CreateDescriptorHeap(GraphicsDevice& Device);
		void QueueUploadBufferForEviction(GraphicsDevice& Device) const;
		void CreateTextureBuffers(GraphicsDevice& Device, GraphicsCommandList& CommandList);
		DXGI_FORMAT DetermineTextureDataFormat() const;
		void UploadTextureData(GraphicsCommandList& CommandList);
		static UINT64 CalculateBufferSize(TextureData& Data);
	protected:
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override;
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override;
		D3D12_RESOURCE_DESC CreateResourceDescription(D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE) override;
	};

}
