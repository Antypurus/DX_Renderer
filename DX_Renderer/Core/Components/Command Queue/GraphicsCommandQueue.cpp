#include "GraphicsCommandQueue.hpp"

#include "../../../Tooling/Validate.hpp"
#include "../Command List/GraphicsCommandList.hpp"
#include "../GraphicsDevice.hpp"

namespace DXR
{
	GraphicsCommandQueue::GraphicsCommandQueue(GraphicsDevice& device) :CommandQueue(CommandQueueType::Direct)
	{
		this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		this->CreateCommandQueue(device);
	}

	GraphicsCommandQueue::GraphicsCommandQueue(const GraphicsCommandQueue& queue) :CommandQueue(queue.Type)
	{
		this->m_command_queue_type = queue.m_command_queue_type;
		this->m_command_queue = queue.m_command_queue;
	}

	void GraphicsCommandQueue::ExecuteCommandList(const GraphicsCommandList& CommandList)
	{
		ID3D12CommandList* list[] = { CommandList.GetRAWInterface() };
		this->m_command_queue->ExecuteCommandLists(1, list);
	}

	void GraphicsCommandQueue::ExecuteCommandLists(const std::vector<GraphicsCommandList*>& CommandLists)
	{
		ID3D12CommandList** lists = new ID3D12CommandList * [CommandLists.size()];
		for (size_t list_index = 0; list_index < CommandLists.size(); ++list_index)
		{
			lists[list_index] = CommandLists[list_index]->GetRAWInterface();
		}
		this->m_command_queue->ExecuteCommandLists(CommandLists.size(), lists);
		delete[] lists;
	}

	inline void GraphicsCommandQueue::CreateCommandQueue(GraphicsDevice& device)
	{
		D3D12_COMMAND_QUEUE_DESC command_queue_description = {};
		command_queue_description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		command_queue_description.Type = this->m_command_queue_type;
		INFO_LOG(L"Creating Command Queue");
		DXCall(device->CreateCommandQueue(&command_queue_description, IID_PPV_ARGS(&this->m_command_queue)));
		SUCCESS_LOG(L"Command Queue Created");
	}
}