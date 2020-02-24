#include "RenderTargetView.h"

namespace DXR
{
	RenderTargetView::RenderTargetView(ID3D12Resource* RenderTarget, DescriptorHeap& DescriptorHeap,
		Resolution& Resolution):Resource(DescriptorHeap)
	{
		this->m_resource = WRL::ComPtr<ID3D12Resource>(RenderTarget);
	}
}
