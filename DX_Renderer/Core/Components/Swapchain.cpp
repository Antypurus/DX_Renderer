#include "Swapchain.hpp"
#include "../../Tooling/Validate.hpp"
#include "GraphicsDevice.hpp"
#include "Command List/GraphicsCommandList.hpp"

namespace DXR
{
	IDXGISwapChain* Swapchain::operator->()
	{
		return this->m_swapchain.Get();
	}

	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, GraphicsCommandList& commandList)
		:m_resolution(window.GetResolution()), m_refresh_rate(refreshRate)
	{
		this->Create(device, window, commandList);
	}

	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, GraphicsCommandList& commandList, DXGI_FORMAT backbufferFormat)
		: m_resolution(window.GetResolution()), m_refresh_rate(refreshRate), m_backbuffer_format(backbufferFormat)
	{
		this->Create(device, window, commandList);
	}

	void Swapchain::Create(GraphicsDevice& device, Window& window,
		GraphicsCommandList& commandList)
	{
		device.CheckSupportedMSAALevels(this->m_backbuffer_format);
		this->m_RTV_descriptor_heap = device.CreateRenderTargetViewDescriptorHeap(this->m_swapchain_buffer_count);
		this->m_DSV_descriptor_heap = device.CreateDepthStencilBufferDescriptorHeap(1);
		this->CreateSwapChain(device, window);
		this->CreateRenderTargetViews(device);
		this->CreateDepthStencilBufferView(device, commandList);
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

	void Swapchain::CreateRenderTargetViews(GraphicsDevice& device) const
	{
		for(UINT i = 0;i < this->m_swapchain_buffer_count;++i)
		{
			WRL::ComPtr<ID3D12Resource> backbuffer;
			DXCall(this->m_swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer)));
			device->CreateRenderTargetView(backbuffer.Get(), nullptr, this->m_RTV_descriptor_heap[i]);
		}
	}

	void Swapchain::CreateDepthStencilBufferView(GraphicsDevice& device, GraphicsCommandList& commandList)
	{
		this->m_depth_stencil_buffer_resource = std::make_unique<DepthStencilBuffer>(device, commandList, this->m_DSV_descriptor_heap, this->m_resolution);
	}

	void Swapchain::SetViewport(GraphicsCommandList& commandList, Resolution& resolution, UINT xOffset, UINT yOffset)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.Width = resolution.Width;
		viewport.Height = resolution.Height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = xOffset;
		viewport.TopLeftY = yOffset;

		commandList->RSSetViewports(1, &viewport);
	}

	void Swapchain::Present(GraphicsCommandList& commandList)
	{
		INFO_LOG(L"Swapchain Update\n");
		this->m_swapchain->Present(0, 0);
		this->m_current_backbuffer = this->m_backbuffer_format == 1 ? 0 : 1;
	}
}
