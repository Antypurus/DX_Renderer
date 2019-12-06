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
		return {};
	}

	D3D12_HEAP_PROPERTIES ConstantBuffer::CreateResourceHeapDescription()
	{
		D3D12_HEAP_PROPERTIES heap_properties = {};
		
		return heap_properties;
	}
}
