#pragma once

#include <d3d12.h>

namespace DXR
{

	struct GraphicsCommandList;
	
	struct TransitionResourceBarrier
	{
	public:
	private:
		D3D12_RESOURCE_STATES m_initial_state;
		D3D12_RESOURCE_STATES m_after_state;
		ID3D12Resource* m_resource;
		D3D12_RESOURCE_BARRIER m_resource_barrier = {};
	public:
		TransitionResourceBarrier(ID3D12Resource& resource,D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_STATES afterState);
		void ExecuteResourceBarrier(GraphicsCommandList& commandList) const;
	private:
		inline void CreateResourceBarrier();
	};

	struct UAVResourceBarrier
	{
	public:
	private:
		ID3D12Resource* m_resource;
		D3D12_RESOURCE_BARRIER m_resource_barrier = {};
	public:
		UAVResourceBarrier(ID3D12Resource& resource);
		void ExecuteResourceBarrier(GraphicsCommandList& commandList) const;
	private:
		inline void CreateResourceBarrier();
	};

}
