#pragma once

#include "Resource.hpp"
#include "../../Windows Abstractions/Window.hpp"

namespace DXR
{

	struct GraphicsDevice;
	struct GraphicsCommandList;
	struct DescriptorHeap;
	
	class DepthStencilBuffer : public Resource
	{
	public:
		const DXGI_FORMAT DepthStencilBufferFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	private:
		Resolution m_resolution;
	public:
		DepthStencilBuffer(GraphicsDevice& device, GraphicsCommandList& commandList,DescriptorHeap& heap,Resolution& resolution);
	protected:
		D3D12_RESOURCE_DESC CreateResourceDescription() override;
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override;
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override;
	};
}
