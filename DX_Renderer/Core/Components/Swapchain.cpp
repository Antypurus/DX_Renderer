#include "Swapchain.hpp"
#include "../../Tooling/Validate.hpp"

namespace DXR
{
	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate)
		:m_resolution(window.GetResolution()), m_refresh_rate(refreshRate)
	{
		device.CheckSupportedMSAALevels(this->m_backbuffer_format);
		this->CreateSwapChain(device, window);
	}

	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, DXGI_FORMAT backbufferFormat)
		: m_resolution(window.GetResolution()), m_refresh_rate(refreshRate), m_backbuffer_format(backbufferFormat)
	{
		device.CheckSupportedMSAALevels(this->m_backbuffer_format);
		this->CreateSwapChain(device, window);
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
		swapchain_description.SampleDesc.Count = 1;
		swapchain_description.SampleDesc.Quality = 0;
		swapchain_description.BufferDesc = swapchain_buffer_description;
		swapchain_description.BufferCount = this->m_swapchain_buffer_count;
		swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapchain_description.Windowed = this->m_windowed_mode ? 1 : 0;// convert from C++ bool to a WinAPI BOOL
		swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_description.OutputWindow = window.GetWindowHandle();

		DXCall(device.GetDXGIFactory()->CreateSwapChain(device.GetGraphicsCommandQueue()->GetCommandQueueRawPtr(), &swapchain_description, this->m_swapchain.GetAddressOf()));
	}
}
