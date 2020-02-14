#include "Swapchain.hpp"
#include "../../Tooling/Validate.hpp"
#include "GraphicsDevice.hpp"
#include "Command List/GraphicsCommandList.hpp"
#include "Resource/ResourceBarrier.hpp"

namespace DXR
{
	IDXGISwapChain1* Swapchain::operator->()
	{
		return this->m_swapchain.Get();
	}

	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, GraphicsCommandList& commandList)
		:m_resolution(window.GetResolution()), m_refresh_rate(refreshRate)
	{
		this->Create(device, window, commandList);
	}

	Swapchain::Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, GraphicsCommandList& commandList, DXGI_FORMAT backbufferFormat)
		: m_resolution(window.GetResolution()), m_refresh_rate(refreshRate)
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
		INFO_LOG(L"Swapchain ComPtr references reset");

		// description of the swapchain
		WRL::ComPtr<IDXGISwapChain1> swapchain;
		DXGI_SWAP_CHAIN_DESC1 swapchain_description = {};
		swapchain_description.BufferCount = this->m_swapchain_buffer_count;
		swapchain_description.Width = this->m_resolution.Width;
		swapchain_description.Height = this->m_resolution.Height;
		swapchain_description.Format = this->m_backbuffer_format;
		swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_description.SampleDesc.Count = 1;

		INFO_LOG(L"Creating Swapchain");
		DXCall(device.GetDXGIFactory()->CreateSwapChainForHwnd(device.GetGraphicsCommandQueue().GetCommandQueueRawPtr()
															   , window.GetWindowHandle(),
															   &swapchain_description,
															   nullptr,
															   nullptr,
															   &swapchain));
		SUCCESS_LOG(L"Swapchain Created");
		DXCall(device.GetDXGIFactory()->MakeWindowAssociation(window.GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));
		swapchain.As(&this->m_swapchain);
	}

	void Swapchain::CreateRenderTargetViews(GraphicsDevice& device)
	{
		this->m_backbuffers = {nullptr,nullptr};
		for(UINT i = 0;i < this->m_swapchain_buffer_count;++i)
		{
			INFO_LOG(L"Creating Backbuffer Render Target View");
			DXCall(this->m_swapchain->GetBuffer(i, IID_PPV_ARGS(&this->m_backbuffers[i])));
			device->CreateRenderTargetView(this->m_backbuffers[i].Get(), nullptr, this->m_RTV_descriptor_heap[i]);
			SUCCESS_LOG(L"Backbuffer Render Target View Created");
		}
	}

	void Swapchain::CreateDepthStencilBufferView(GraphicsDevice& device, GraphicsCommandList& commandList)
	{
		this->m_depth_stencil_buffer_resource = std::make_unique<DepthStencilBuffer>(device, commandList, this->m_DSV_descriptor_heap, this->m_resolution);
	}

	void Swapchain::SetViewport(GraphicsCommandList& commandList, Resolution& resolution, UINT xOffset, UINT yOffset)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.Width = (FLOAT)resolution.Width;
		viewport.Height = (FLOAT)resolution.Height;
		viewport.MinDepth = D3D12_MIN_DEPTH;
		viewport.MaxDepth = D3D12_MAX_DEPTH;
		viewport.TopLeftX = (FLOAT)xOffset;
		viewport.TopLeftY = (FLOAT)yOffset;

		commandList->RSSetViewports(1, &viewport);
	}

	void Swapchain::SetScisorRect(GraphicsCommandList& commandList, Resolution& resolution)
	{
		D3D12_RECT rect = {};
		rect.left = 0;
		rect.top = 0;
		rect.right = resolution.Width;
		rect.bottom = resolution.Height;

		commandList->RSSetScissorRects(1, &rect);
	}

	void Swapchain::Prepare(GraphicsCommandList& commandList)
	{
		this->SetViewport(commandList, this->m_resolution, 0, 0);
		this->SetScisorRect(commandList, this->m_resolution);

		ResourceBarrier barrier = {*this->m_backbuffers[this->m_current_backbuffer].Get(),D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET};
		barrier.ExecuteResourceBarrier(commandList);
	}

	void Swapchain::Present(GraphicsCommandList& commandList)
	{
		DXCall(this->m_swapchain->Present(0, 0));
		this->m_current_backbuffer = this->m_current_backbuffer == 1 ? 0 : 1;
	}

	ID3D12Resource* Swapchain::GetCurrentBackbuffer()
	{
		return this->m_backbuffers[this->m_current_backbuffer].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::GetCurrentBackBufferDescriptor()
	{
		return this->m_RTV_descriptor_heap[this->m_current_backbuffer];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::GetDepthStencilBufferDescriptor()
	{
		return this->m_DSV_descriptor_heap[0];
	}

	void Swapchain::PrepareBackbufferForPresentation(GraphicsCommandList& commandList)
	{
		ResourceBarrier barrier = {*this->m_backbuffers[this->m_current_backbuffer].Get(),D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_PRESENT};
		barrier.ExecuteResourceBarrier(commandList);
	}
}
