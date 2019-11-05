#include "GraphicsCommandList.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
	GraphicsCommandList::GraphicsCommandList(GraphicsDevice& device)
	{
		this->CreateCommandAllocator(device);
		this->CreateCommandList(device);
	}

	ID3D12GraphicsCommandList* GraphicsCommandList::operator->()
	{
		return this->m_command_list.Get();
	}

	inline void GraphicsCommandList::CreateCommandAllocator(GraphicsDevice& device)
	{
		DXCall(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->m_command_allocator)));
	}

	inline void GraphicsCommandList::CreateCommandList(GraphicsDevice& device)
	{
		DXCall(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->m_command_allocator.Get(),nullptr,IID_PPV_ARGS(&this->m_command_list)));
	}
}