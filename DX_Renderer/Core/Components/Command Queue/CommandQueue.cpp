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

	void CommandQueue::Flush(Fence fence)
	{
		fence.Advance();
		fence.Signal(*this);
		fence.WaitForFence();
	}

	CommandQueue::CommandQueue(CommandQueueType type): Type(type), m_command_queue_type()
	{
	}
}
