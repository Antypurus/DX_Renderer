#pragma once
#include "Resource.hpp"

namespace DXR
{
	struct GraphicsCommandList;
	struct Resolution;

	using namespace Microsoft;
	
	struct RenderTargetView : public Resource
	{
	public:
	private:
	public:
		RenderTargetView(WRL::ComPtr<ID3D12Resource>& RenderTarget, DescriptorHeap& DescriptorHeap, Resolution& Resolution, size_t HeapIndex);
		void Clear(GraphicsCommandList& CommandList, FLOAT ClearColor[4]) const;
	protected:
		D3D12_RESOURCE_DESC CreateResourceDescription() override { return {}; };		//Todo(Tiago): Placeholder
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override { return {}; };			//Todo(Tiago): Placeholder
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override { return {}; };	//Todo(Tiago): Placeholder
	private:
	};
}