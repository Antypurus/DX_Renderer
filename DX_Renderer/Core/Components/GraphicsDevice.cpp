#include "GraphicsDevice.hpp"
#include "../../Tooling/Validate.hpp"
#include "Fence.hpp"

namespace DXR
{
	GraphicsDevice::GraphicsDevice()
	{
		this->CreateDXGIFactory();
		this->CreateDefaultD3D12Device();
		this->QueryAllDescriptorSizes();
	}

	GraphicsDevice::GraphicsDevice(UINT8 DeviceIndex)
	{
		this->CreateDXGIFactory();
		this->CreateD3D12Device(DeviceIndex);
		this->QueryAllDescriptorSizes();
	}

	ID3D12Device* GraphicsDevice::operator->()
	{
		return this->m_device.Get();
	}

	Fence GraphicsDevice::CreateFence(UINT64 initialValue)
	{
		return Fence(initialValue, *this);
	}

	void GraphicsDevice::CreateDXGIFactory()
	{
		DXCall(::CreateDXGIFactory(IID_PPV_ARGS(&this->m_dxgi_factory)));
	}

	void GraphicsDevice::CreateDefaultD3D12Device()
	{
		DXCall(D3D12CreateDevice(nullptr, this->m_minimum_feature_level, IID_PPV_ARGS(&this->m_device)));
	}

	void GraphicsDevice::CreateD3D12Device(UINT8 deviceIndex)
	{
		auto adapter_list = this->GetGraphicsAdapterList();
		DXCall(D3D12CreateDevice(adapter_list[deviceIndex].Get(), this->m_minimum_feature_level, IID_PPV_ARGS(&this->m_device)));
	}

	std::vector<WRL::ComPtr<IDXGIAdapter>> GraphicsDevice::GetGraphicsAdapterList() const
	{
		std::vector<WRL::ComPtr<IDXGIAdapter>> adapter_list;
		UINT8 adapter_index = 0;
		IDXGIAdapter* current_adapter = nullptr;
		while(m_dxgi_factory->EnumAdapters(adapter_index, &current_adapter) != DXGI_ERROR_NOT_FOUND)
		{
			adapter_list.emplace_back(current_adapter);

			{
				// to prevent leaks we must release the adapter interface when we no longer need it
				current_adapter->Release();
				current_adapter = nullptr;
			}

			++adapter_index;
		}
		return adapter_list;
	}

	void GraphicsDevice::QueryAllDescriptorSizes()
	{
		const UINT64 RTV_size = this->QueryDescriptoSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		const UINT64 CBV_SRV_UAV_size = this->QueryDescriptoSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const UINT64 DSV_size = this->QueryDescriptoSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		const UINT64 Sampler_size = this->QueryDescriptoSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		this->descriptorSizes = {RTV_size,CBV_SRV_UAV_size,DSV_size,Sampler_size};
	}

	UINT64 GraphicsDevice::QueryDescriptoSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorType)
	{
		return this->m_device->GetDescriptorHandleIncrementSize(descriptorType);
	}
}
