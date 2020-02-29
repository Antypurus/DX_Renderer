#include "RenderTargetView.hpp"
#include "../Command List/GraphicsCommandList.hpp"
#include "DescriptorHeap.hpp"

namespace DXR
{
	RenderTargetView::RenderTargetView(ID3D12Resource* RenderTarget, DescriptorHeap& DescriptorHeap,
		Resolution& Resolution, size_t HeapIndex):Resource(DescriptorHeap, HeapIndex)
	{
		this->m_resource = WRL::ComPtr<ID3D12Resource>(RenderTarget);
		this->m_descriptor_heap = &DescriptorHeap;
		this->m_heap_index = HeapIndex;
	}

	void RenderTargetView::Clear(GraphicsCommandList& CommandList, FLOAT ClearColor[4]) const
	{
		CommandList->ClearRenderTargetView((*this->m_descriptor_heap)[this->m_heap_index], ClearColor, 0, nullptr);
	}
}
