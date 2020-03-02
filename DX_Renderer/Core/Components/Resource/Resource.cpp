#include "Resource.hpp"
#include "DescriptorHeap.hpp"

namespace DXR
{
	ID3D12Resource* Resource::operator->() const
	{
		return this->m_resource.Get();
	}

	ID3D12Resource* Resource::GetResource() const
	{
		return this->m_resource.Get();
	}

	DescriptorHeap* Resource::GetDescriptorHeap()
	{
		return this->m_descriptor_heap;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Resource::GetCPUHandle() const
	{
		return (*this->m_descriptor_heap)[this->m_heap_index];
	}

	Resource::Resource(DescriptorHeap& heap, size_t HeapIndex) :m_descriptor_heap(&heap), m_heap_index(HeapIndex){}

	Resource::Resource()
	{
	}
}
