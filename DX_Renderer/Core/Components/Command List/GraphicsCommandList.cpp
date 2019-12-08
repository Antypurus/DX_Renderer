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

	ID3D12GraphicsCommandList* GraphicsCommandList::GetRAWInterface() const
	{
		return this->m_command_list.Get();
	}

	ID3D12CommandAllocator* GraphicsCommandList::GetCommandAllocator()
	{
		return this->m_command_allocator.Get();
	}

	inline void GraphicsCommandList::CreateCommandAllocator(GraphicsDevice& device)
	{
		INFO_LOG(L"Creating Command Allocator");
		DXCall(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->m_command_allocator)));
		SUCCESS_LOG(L"Command Allocator Created");
	}

	inline void GraphicsCommandList::CreateCommandList(GraphicsDevice& device)
	{
		INFO_LOG(L"Creating Graphics Command List");
		DXCall(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->m_command_allocator.Get(),nullptr,IID_PPV_ARGS(&this->m_command_list)));
		SUCCESS_LOG(L"Graphics Command List Created");
	}
}