#pragma once

#include <d3d12.h>
#include <memory>
#include "../Resource/DescriptorHeap.hpp"

namespace DXR
{

	struct Swapchain;
	struct GraphicsDevice;
	struct GraphicsCommandList;
	struct GPUDefaultBuffer;

	struct RayTracingOutput
	{
	public:
	private:
		DXGI_FORMAT m_texture_format;
		UINT m_height;
		UINT m_width;
		D3D12_RESOURCE_DESC m_texture_desc = {};
		DescriptorHeap m_heap;
		std::unique_ptr<GPUDefaultBuffer> m_texture_buffer;
	public:
		RayTracingOutput(GraphicsDevice& Device, GraphicsCommandList& CommandList, Swapchain& swapchain);
	private:
		void CreateResourceDescription();
		void CreateShaderResourceViewDescription(GraphicsDevice& Device) const;
		void CreateTextureBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
		void CreateUAVDescriptor(GraphicsDevice& Device);
	};
}