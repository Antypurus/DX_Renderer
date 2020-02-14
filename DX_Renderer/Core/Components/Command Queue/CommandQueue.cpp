#include "CommandQueue.hpp"
#include "../Fence.hpp"

namespace DXR
{
	ID3D12CommandQueue* CommandQueue::operator->()
	{
		return this->m_command_queue.Get();
	}

	ID3D12CommandQueue* CommandQueue::GetCommandQueueRawPtr()
	{
		return this->m_command_queue.Get();
	}

	void CommandQueue::Flush(Fence& fence)
	{
		fence.Advance();
		fence.Signal(*this);
		fence.WaitForFence();
	}

	CommandQueue::CommandQueue(CommandQueueType type) : Type(type)
	{
		this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		switch(Type)
		{
			case(CommandQueueType::Direct):
			{
				this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			}
			case(CommandQueueType::Bundle):
			{
				this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_BUNDLE;
				break;
			}
			case(CommandQueueType::Compute):
			{
				this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			}
			default:break;
		}
	}
}
