#pragma once
#include "../Resource/Texture/Texture.hpp"
#include "../Swapchain.hpp"

namespace DXR
{
	struct RayTracingOutput:public Texture
	{
	public:
	private:
	public:
		RayTracingOutput(GraphicsDevice& Device, GraphicsCommandList& CommandList, Swapchain& swapchain);
	private:
	};
}