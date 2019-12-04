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

	Resource::Resource(DescriptorHeap& heap) :m_descriptor_heap(&heap){}

	Resource::Resource()
	{
	}
}
