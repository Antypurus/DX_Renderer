#include "ResourceBarrier.hpp"

#include "../Command List/GraphicsCommandList.hpp"
#include "../../../Tooling/Log.hpp"

namespace DXR
{
	ResourceBarrier::ResourceBarrier(ID3D12Resource& resource, D3D12_RESOURCE_STATES initialState,
		D3D12_RESOURCE_STATES afterState):m_initial_state(initialState),m_after_state(afterState),m_resource(&resource)
	{
		this->CreateResourceBarrier();
	}

	void ResourceBarrier::CreateResourceBarrier()
	{
		this->m_resource_barrier = {};
		this->m_resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		this->m_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		this->m_resource_barrier.Transition.StateBefore = this->m_initial_state;
		this->m_resource_barrier.Transition.StateAfter = this->m_after_state;
		this->m_resource_barrier.Transition.pResource = m_resource;
		this->m_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	}

	void ResourceBarrier::ExecuteResourceBarrier(GraphicsCommandList& commandList) const
	{
		commandList->ResourceBarrier(1,&this->m_resource_barrier);
		INFO_LOG(L"Queued Resource Barrier Into Command Queue");
	}
}
