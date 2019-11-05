#pragma once

#include "CommandQueue.hpp"
#include "../GraphicsDevice.hpp"

namespace DXR
{
	struct GraphicsCommandQueue: public CommandQueue
	{
	public:
		GraphicsCommandQueue(GraphicsDevice& device);
	private:
		inline void CreateCommandQueue(GraphicsDevice& device);
	};
}
