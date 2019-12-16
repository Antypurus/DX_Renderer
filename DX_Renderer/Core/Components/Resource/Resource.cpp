#include "Resource.hpp"

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

	Resource::Resource(DescriptorHeap& heap) :m_descriptor_heap(&heap){}

	Resource::Resource()
	{
	}
}
