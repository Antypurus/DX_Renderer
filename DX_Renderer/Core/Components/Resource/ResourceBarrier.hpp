#pragma once

#include <d3d12.h>

namespace DXR
{

	struct GraphicsCommandList;
	
	struct ResourceBarrier
	{
	public:
	private:
		D3D12_RESOURCE_STATES m_initial_state;
		D3D12_RESOURCE_STATES m_after_state;
		ID3D12Resource* m_resource;
		D3D12_RESOURCE_BARRIER m_resource_barrier = {};
	public:
		ResourceBarrier(ID3D12Resource& resource,D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_STATES afterState);
		void ExecuteResourceBarrier(GraphicsCommandList& commandList) const;
	private:
		inline void CreateResourceBarrier();
	};
	
}
