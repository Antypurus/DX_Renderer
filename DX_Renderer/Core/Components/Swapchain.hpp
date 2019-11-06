#pragma once

#include <wrl.h>
#include <dxgi.h>
#include "../Windows Abstractions/Window.hpp"
#include "GraphicsDevice.hpp"

namespace DXR
{
	using namespace Microsoft;

	struct Swapchain
	{
	public:
	private:
		bool m_use_msaa = true;
		bool m_windowed_mode = true;
		const UINT8 m_swapchain_buffer_count = 2;
		Resolution m_resolution;
		UINT16 m_refresh_rate;
		WRL::ComPtr<IDXGISwapChain> m_swapchain;
		DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	public:
		Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate);
		Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, DXGI_FORMAT backbufferFormat);
	private:
		inline void CreateSwapChain(GraphicsDevice& device, Window& window);
	};
}
