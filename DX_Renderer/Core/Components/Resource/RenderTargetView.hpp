#pragma once
#include "Resource.hpp"

namespace DXR
{
	struct Resolution;

	class RenderTargetView : public DXR::Resource
	{
	public:
	private:
	public:
		RenderTargetView(ID3D12Resource* RenderTarget, DescriptorHeap& DescriptorHeap, Resolution& Resolution, size_t HeapIndex);
	private:
	};
}