#pragma once

#include <dxgi.h>
#include <wrl.h>
#include <vector>
#include <d3d12.h>

namespace DXR
{
	using namespace Microsoft;
	
	struct Fence;

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
		D3D_FEATURE_LEVEL m_minimum_feature_level = D3D_FEATURE_LEVEL_11_0;
		
		WRL::ComPtr<IDXGIFactory> m_dxgi_factory;
		WRL::ComPtr <ID3D12Device> m_device;

		DescriptorSizes descriptorSizes;

	// public and privte methods
	public:
		GraphicsDevice();
		GraphicsDevice(UINT8 DeviceIndex);
		ID3D12Device* operator->();
		Fence CreateFence(UINT64 initialValue);
		void CheckSupportedMSAALevels(DXGI_FORMAT backbufferFormat);
	private:
		void CreateDXGIFactory();
		void CreateDefaultD3D12Device();
		void CreateD3D12Device(UINT8 deviceIndex);
		std::vector<WRL::ComPtr<IDXGIAdapter>> GetGraphicsAdapterList() const;
		void QueryAllDescriptorSizes();
		UINT64 QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorType);
	};
}
