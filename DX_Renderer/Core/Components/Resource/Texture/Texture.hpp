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
    
	struct Texture : public Resource
	{
        public:
		friend GraphicsCommandList;
        private:
		Sampler m_sampler;//TODO(Tiago): Unbind sampler from texture
		TextureData m_texture_data;//NOTE(Tiago): Im wasting RAM by keeping the original CPU data after texture upload has occured. I Think at least!?
		DXGI_FORMAT m_texture_format;
		D3D12_RESOURCE_DESC m_resource_description = {};
		D3D12_SHADER_RESOURCE_VIEW_DESC m_srv_desc = {};
		DescriptorHeap m_heap;
		std::shared_ptr<TextureUploadBuffer> m_upload_buffer = nullptr;
		std::shared_ptr<GPUDefaultBuffer> m_texture_buffer = nullptr;
        public:
		Texture() = default;
		Texture(const std::wstring& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList);
        Texture(const Texture& other);
		DescriptorHeap* GetSRVHeap();
		DescriptorHeap* GetSamplerHeap();
        protected:
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
