#include "RenderTargetView.hpp"

namespace DXR
{
	RenderTargetView::RenderTargetView(ID3D12Resource* RenderTarget, DescriptorHeap& DescriptorHeap,
		Resolution& Resolution, size_t HeapIndex):Resource(DescriptorHeap, HeapIndex)
	{
		this->m_resource = WRL::ComPtr<ID3D12Resource>(RenderTarget);
	}
}
