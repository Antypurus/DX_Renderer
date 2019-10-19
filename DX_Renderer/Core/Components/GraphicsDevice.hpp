#pragma once

#include <dxgi.h>
#include <wrl.h>
#include <vector>

namespace DXR
{
	using namespace Microsoft;
	
	struct GraphicsDevice
	{
	//
	public:
		
	private:
		WRL::ComPtr<IDXGIFactory> m_dxgi_factory;
	//
	public:
		GraphicsDevice();
		GraphicsDevice(UINT8 DeviceIndex);
		std::vector<WRL::ComPtr<IDXGIAdapter>> GetGraphicsAdapterList() const;
	private:
		void CreateDXGIFactory();
		
	};
}
