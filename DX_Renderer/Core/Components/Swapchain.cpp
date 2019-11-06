#include "Swapchain.hpp"

namespace DXR
{
	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate)
		:m_resolution(window.GetResolution()), m_refresh_rate(refreshRate)
	{
	}

	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, DXGI_FORMAT backbufferFormat)
		: m_resolution(window.GetResolution()), m_refresh_rate(refreshRate), m_backbuffer_format(backbufferFormat)
	{
	}

	inline void Swapchain::CreateSwapChain(GraphicsDevice& device, Window& window)
	{
		this->m_swapchain.Reset();

		// description of the swapchain buffers
		DXGI_MODE_DESC swapchain_buffer_description = {};
		swapchain_buffer_description.Width = this->m_resolution.Width;
		swapchain_buffer_description.Height = this->m_resolution.Height;
		swapchain_buffer_description.Format = this->m_backbuffer_format;
		swapchain_buffer_description.RefreshRate.Numerator = this->m_refresh_rate;
		swapchain_buffer_description.RefreshRate.Denominator = 1;
		swapchain_buffer_description.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapchain_buffer_description.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		// description of the swapchain
		DXGI_SWAP_CHAIN_DESC swapchain_description = {};

	}
}
