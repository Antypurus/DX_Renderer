#include "ConstantBuffer.hpp"

namespace DXR
{
	D3D12_RESOURCE_DESC ConstantBuffer::CreateResourceDescription()
	{
		D3D12_RESOURCE_DESC resource_description = {};

		return resource_description;
	}

	D3D12_CLEAR_VALUE ConstantBuffer::CreateOptimizedClearValue()
	{
	}

	D3D12_HEAP_PROPERTIES ConstantBuffer::CreateResourceHeapDescription()
	{
	}
}
