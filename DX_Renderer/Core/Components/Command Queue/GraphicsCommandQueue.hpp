#pragma once

#include "CommandQueue.hpp"
#include "../GraphicsDevice.hpp"

namespace DXR
{
	struct GraphicsCommandQueue: public CommandQueue
	{
	public:
		friend GraphicsDevice;
	private:
		GraphicsCommandQueue(GraphicsDevice& device);
		inline void CreateCommandQueue(GraphicsDevice& device);
	};
}
