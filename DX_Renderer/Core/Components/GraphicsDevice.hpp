#pragma once

#include <dxgi.h>
#include <wrl.h>
#include <vector>
#include <d3d12.h>

#include "Command Queue/CommandQueue.hpp"

namespace DXR
{
	struct Window;
	using namespace Microsoft;

	struct Fence;
	struct GraphicsCommandList;
	struct Swapchain;
	struct DescriptorHeap;

	struct DescriptorSizes
	{
		UINT64 RTV;
		UINT64 CBV_SRV_UAV;
		UINT64 DSV;
		UINT64 Sampler;
	};

	struct GraphicsDevice
	{
		// public and private data fields
	public:
		std::vector<UINT8> supported_mssa_levels;
	private:
		enum D3D_FEATURE_LEVEL m_minimum_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
		WRL::ComPtr<IDXGIFactory> m_dxgi_factory;
		WRL::ComPtr <ID3D12Device> m_device;
		DescriptorSizes descriptorSizes{};
		CommandQueue* m_graphics_command_queue{};
		// public and privte methods
	public:
		GraphicsDevice();
		GraphicsDevice(UINT8 DeviceIndex);
		ID3D12Device* operator->() const;
		IDXGIFactory* GetDXGIFactory() const;
		DescriptorSizes GetDescriptorSizes()const;
		void CheckSupportedMSAALevels(DXGI_FORMAT backbufferFormat);
		CommandQueue* GetGraphicsCommandQueue();
		Fence CreateFence(UINT64 initialValue);
		GraphicsCommandList CreateGraphicsCommandList();
		Swapchain CreateSwapchain(Window& window, UINT refreshRate, GraphicsCommandList& commandList);
		DescriptorHeap CreateRenderTargetViewDescriptorHeap(const UINT descriptorCount);
		DescriptorHeap CreateDepthStencilBufferDescriptorHeap(const UINT descriptorCount);
	private:
		void CreateDXGIFactory();
		void CreateDefaultD3D12Device();
		void CreateD3D12Device(UINT8 deviceIndex);
		std::vector<WRL::ComPtr<IDXGIAdapter>> GetGraphicsAdapterList() const;
		void QueryAllDescriptorSizes();
		UINT64 QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorType) const;
		inline void CreateGraphicsCommandQueue();
	};
}
