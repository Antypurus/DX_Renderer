#pragma once

#include <dxgi1_4.h>
#include <wrl.h>
#include <vector>
#include <d3d12.h>
#include <d3d12video.h>
#include <string>

#include "Command Queue/GraphicsCommandQueue.hpp"

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
        std::wstring name = L"";
		std::vector<UINT8> supported_mssa_levels;
		bool supports_ray_tracing = false;
		static GraphicsDevice* Device;
        private:
		enum D3D_FEATURE_LEVEL m_minimum_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0;
		WRL::ComPtr<IDXGIFactory2> m_dxgi_factory;
		WRL::ComPtr<ID3D12Device5> m_device;
        WRL::ComPtr<ID3D12VideoDevice1> m_video_device;
		DescriptorSizes descriptorSizes{};
		GraphicsCommandQueue* m_graphics_command_queue{};
		// public and private methods
        public:
		GraphicsDevice(UINT8 DeviceIndex = 0);
		ID3D12Device5* operator->() const;
		IDXGIFactory2* GetDXGIFactory() const;
		DescriptorSizes GetDescriptorSizes()const;
		void CheckSupportedMSAALevels(DXGI_FORMAT backbufferFormat);
		GraphicsCommandQueue& GetGraphicsCommandQueue();
		Fence CreateFence(UINT64 initialValue);
		GraphicsCommandList CreateGraphicsCommandList();
		Swapchain CreateSwapchain(Window& window, UINT refreshRate, GraphicsCommandList& commandList);
		DescriptorHeap CreateRenderTargetViewDescriptorHeap(const UINT descriptorCount);
		DescriptorHeap CreateDepthStencilBufferDescriptorHeap(const UINT descriptorCount);
		DescriptorHeap CreateConstantBufferDescriptorHeap(const UINT descriptorCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		DescriptorHeap CreateShaderResourceDescriptorHeap(const UINT descriptorCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		DescriptorHeap CreateSamplerDescriptorHeap(const UINT descriptorCount, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		ID3D12Device5* GetRawInterface() const;
        ID3D12VideoDevice1* GetVideoDevice() const;
        private:
		bool QueryRayTracingSupport() const;
        void CreateVideoDevice();
		void CreateDXGIFactory();
		void CreateDefaultD3D12Device();
		void CreateD3D12Device(UINT8 deviceIndex);
		std::vector<WRL::ComPtr<IDXGIAdapter>> GetGraphicsAdapterList() const;
		void QueryAllDescriptorSizes();
		UINT64 QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorType) const;
		inline void CreateGraphicsCommandQueue();
	};
}
