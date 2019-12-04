#pragma once
#include "../Resource.hpp"

namespace DXR
{
	struct GPUDefaultBuffer:public Resource
	{
	public:
	private:
		UINT64 m_element_count = 0;
		UINT64 m_element_size = 0;
	public:
		GPUDefaultBuffer(UINT64 elementCount,UINT64 elementSize);
	protected:
		D3D12_RESOURCE_DESC CreateResourceDescription() override;
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override;
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override;
	};
}
