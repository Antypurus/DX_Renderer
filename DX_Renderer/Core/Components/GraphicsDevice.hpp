#pragma once

#include <dxgi.h>
#include <wrl.h>
#include <vector>
#include <d3d12.h>

namespace DXR
{
	using namespace Microsoft;
	
	struct GraphicsDevice
	{
	// public and private data fields
	public:
		
	private:
		D3D_FEATURE_LEVEL m_minimum_feature_level = D3D_FEATURE_LEVEL_11_0;
		
		WRL::ComPtr<IDXGIFactory> m_dxgi_factory;
		WRL::ComPtr <ID3D12Device> m_device;

	// public and privte methods
	public:
		GraphicsDevice();
		GraphicsDevice(UINT8 DeviceIndex);
	private:
		void CreateDXGIFactory();
		void CreateDefaultD3D12Device();
		void CreateD3D12Device(UINT8 deviceIndex);
		std::vector<WRL::ComPtr<IDXGIAdapter>> GetGraphicsAdapterList() const;
	};
}
