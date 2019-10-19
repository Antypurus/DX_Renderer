#include "GraphicsDevice.hpp"

namespace DXR
{
	GraphicsDevice::GraphicsDevice()
	{
		this->CreateDXGIFactory();
	}

	void GraphicsDevice::CreateDXGIFactory()
	{
		::CreateDXGIFactory(IID_PPV_ARGS(&this->m_dxgi_factory));
	}

	std::vector<WRL::ComPtr<IDXGIAdapter>> GraphicsDevice::GetGraphicsAdapterList() const
	{
		std::vector<WRL::ComPtr<IDXGIAdapter>> adapter_list;
		UINT8 adapter_index = 1;
		IDXGIAdapter* current_adapter = nullptr;
		while(m_dxgi_factory->EnumAdapters(adapter_index,&current_adapter) != DXGI_ERROR_NOT_FOUND)
		{
			adapter_list.emplace_back(current_adapter);

			{
				// to prevent leaks we must release the adapter interface when we no longer need;
				current_adapter->Release();
				current_adapter = nullptr;
			}
			
			++adapter_index;
		}
		return adapter_list;
	}
}
