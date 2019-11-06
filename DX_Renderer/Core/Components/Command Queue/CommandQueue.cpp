#include "CommandQueue.hpp"

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
}