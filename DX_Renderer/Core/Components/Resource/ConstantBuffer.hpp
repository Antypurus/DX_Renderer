#pragma once
#include "Resource.hpp"

namespace DXR
{
	template<typename T>
	struct ConstantBuffer : public Resource
	{
	public:
	private:
		std::unique_ptr<DescriptorHeap> m_descriptor_heap;
		std::vector<T> m_data;
	public:
	protected:
		D3D12_RESOURCE_DESC CreateResourceDescription() override
		{
			D3D12_RESOURCE_DESC
		}

		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override
		{
		}

		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override
		{
		}
	};

}
