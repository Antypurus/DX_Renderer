#pragma once

#include <wrl.h>
#include <d3d12.h>

namespace DXR
{
	using namespace Microsoft;

	enum class CommandQueueType
	{
		None,
		Direct,
		Bundle,
		Compute
	};

	struct CommandQueue
	{
	public:
		const CommandQueueType Type;
	protected:
		D3D12_COMMAND_LIST_TYPE m_command_queue_type;
		WRL::ComPtr<ID3D12CommandQueue> m_command_queue;
	public:
		CommandQueue(CommandQueueType type):Type(type) {};
		virtual ~CommandQueue(){};
		ID3D12CommandQueue* operator->();
	};
}
