#pragma once

#include "CommandQueue.hpp"
#include <vector>

namespace DXR
{
	struct GraphicsCommandList;
	struct GraphicsDevice;
	
	struct GraphicsCommandQueue: public CommandQueue
	{
		friend GraphicsDevice;
	public:
		GraphicsCommandQueue(GraphicsDevice& device);
		GraphicsCommandQueue(const GraphicsCommandQueue& queue);
		void ExecuteCommandList(const GraphicsCommandList& CommandList);
		void ExecuteCommandLists(const std::vector<GraphicsCommandList&>& CommandLists);
	private:
		inline void CreateCommandQueue(GraphicsDevice& device);
	};
}
