#include "GraphicsCommandQueue.hpp"

#include "../../../Tooling/Validate.hpp"

namespace DXR
{
	GraphicsCommandQueue::GraphicsCommandQueue(GraphicsDevice& device):CommandQueue(CommandQueueType::Direct)
	{
		this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		this->CreateCommandQueue(device);
	}

	inline void GraphicsCommandQueue::CreateCommandQueue(GraphicsDevice& device)
	{
		D3D12_COMMAND_QUEUE_DESC command_queue_description = {};
		command_queue_description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		command_queue_description.Type = this->m_command_queue_type;
		DXCall(device->CreateCommandQueue(&command_queue_description, IID_PPV_ARGS(&this->m_command_queue)));
	}
}