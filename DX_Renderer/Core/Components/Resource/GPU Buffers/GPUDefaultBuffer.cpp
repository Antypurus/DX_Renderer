#include "GPUDefaultBuffer.hpp"

namespace DXR
{
	GPUDefaultBuffer::GPUDefaultBuffer(UINT64 elementCount, UINT64 elementSize)
	{
		this->m_element_count = elementCount;
		this->m_element_size = elementSize;
	}

	D3D12_RESOURCE_DESC GPUDefaultBuffer::CreateResourceDescription()
	{
		D3D12_RESOURCE_DESC resource_description = {};
		resource_description.SampleDesc.Count = 1;
		resource_description.SampleDesc.Quality = 0;
		resource_description.Alignment = 0;
		resource_description.MipLevels = 1;
		resource_description.DepthOrArraySize = this->m_element_count;
		resource_description.Width = this->m_element_count * this->m_element_size;
		resource_description.Height = 1;
		resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;
		resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_description.Format = DXGI_FORMAT_UNKNOWN;
		resource_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		return resource_description;
	}

	D3D12_CLEAR_VALUE GPUDefaultBuffer::CreateOptimizedClearValue()
	{
		return {};
	}

	D3D12_HEAP_PROPERTIES GPUDefaultBuffer::CreateResourceHeapDescription()
	{
		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
		return heap_properties;
	}
}
