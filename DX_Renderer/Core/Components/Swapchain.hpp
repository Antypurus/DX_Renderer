#pragma once

#include <vector>
#include <wrl.h>
#include <dxgi.h>
#include <memory>
#include <d3d12.h>
#include "../Windows Abstractions/Window.hpp"
#include "Resource/DescriptorHeap.hpp"
#include "Resource/DepthStencilBuffer.hpp"

namespace DXR
{
	using namespace Microsoft;

	struct GraphicsDevice;
	struct GraphicsCommandList;

	struct Swapchain
	{
	public:
		friend GraphicsDevice;
		const DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	private:
		bool m_windowed_mode = true;
		const UINT8 m_swapchain_buffer_count = 2;
		Resolution m_resolution;
		UINT16 m_refresh_rate;
		WRL::ComPtr<IDXGISwapChain> m_swapchain;
		DescriptorHeap m_RTV_descriptor_heap;
		DescriptorHeap m_DSV_descriptor_heap;
		std::unique_ptr<DepthStencilBuffer> m_depth_stencil_buffer_resource;
		UINT8 m_current_backbuffer = 0;
		std::vector<WRL::ComPtr<ID3D12Resource>> m_backbuffers;
	public:
		IDXGISwapChain* operator->();
		void SetViewport(GraphicsCommandList& commandList, Resolution& resolution, UINT xOffset = 0, UINT yOffset = 0);
		void SetScisorRect(GraphicsCommandList& commandList, Resolution& resolution);
		void Prepare(GraphicsCommandList& commandList);
		void Present(GraphicsCommandList& commandList);
		ID3D12Resource* GetCurrentBackbuffer();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferDescriptor();
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferDescriptor();
		void PrepareBackbufferForPresentation(GraphicsCommandList& commandList);
	private:
		Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, GraphicsCommandList& commandList);
		Swapchain(GraphicsDevice& device, Window& window, UINT16 refreshRate, GraphicsCommandList& commandList, DXGI_FORMAT backbufferFormat);
		inline void Create(GraphicsDevice& device, Window& window, GraphicsCommandList& commandList);
		inline void CreateSwapChain(GraphicsDevice& device, Window& window);
		inline void CreateRenderTargetViews(GraphicsDevice& device);
		inline void CreateDepthStencilBufferView(GraphicsDevice& device, GraphicsCommandList& commandList);
	};
}
