#include "GraphicsDevice.hpp"
#include "../../Tooling/Validate.hpp"
#include "Fence.hpp"
#include "Command Queue/GraphicsCommandQueue.hpp"
#include "Command List/GraphicsCommandList.hpp"
#include "Swapchain.hpp"
#include "Resource/DescriptorHeap.hpp"

namespace DXR
{
	GraphicsDevice::GraphicsDevice()
	{
		this->CreateDXGIFactory();
		this->CreateDefaultD3D12Device();
		this->QueryAllDescriptorSizes();
		this->CreateGraphicsCommandQueue();
	}

	GraphicsDevice::GraphicsDevice(UINT8 DeviceIndex)
	{
		this->CreateDXGIFactory();
		this->CreateD3D12Device(DeviceIndex);
		this->QueryAllDescriptorSizes();
		this->CreateGraphicsCommandQueue();
	}

	ID3D12Device* GraphicsDevice::operator->() const
	{
		return this->m_device.Get();
	}

	IDXGIFactory* GraphicsDevice::GetDXGIFactory() const
	{
		return this->m_dxgi_factory.Get();
	}

	DescriptorSizes GraphicsDevice::GetDescriptorSizes() const
	{
		return this->descriptorSizes;
	}

	Fence GraphicsDevice::CreateFence(UINT64 initialValue)
	{
		return Fence(initialValue, *this);
	}

	GraphicsCommandList GraphicsDevice::CreateGraphicsCommandList()
	{
		return GraphicsCommandList(*this);
	}

	Swapchain GraphicsDevice::CreateSwapchain(Window& window, UINT refreshRate, GraphicsCommandList& commandList)
	{
		return Swapchain(*this, window, refreshRate, commandList);
	}

	DescriptorHeap GraphicsDevice::CreateRenderTargetViewDescriptorHeap(const UINT descriptorCount)
	{
		return DescriptorHeap(*this, descriptorCount, DescriptorType::RenderTargetView);
	}

	DescriptorHeap GraphicsDevice::CreateDepthStencilBufferDescriptorHeap(const UINT descriptorCount)
	{
		return DescriptorHeap(*this, descriptorCount, DescriptorType::DepthStencilBuffer);
	}


	void GraphicsDevice::CreateDXGIFactory()
	{
		INFO_LOG(L"Creating DXGI Factory\n");
		DXCall(::CreateDXGIFactory(IID_PPV_ARGS(&this->m_dxgi_factory)));
		SUCCESS_LOG(L"DXGI Factory Created\n");
	}

	void GraphicsDevice::CreateDefaultD3D12Device()
	{
		INFO_LOG(L"Creating Default D3D12 Device\n");
		DXCall(D3D12CreateDevice(nullptr, this->m_minimum_feature_level, IID_PPV_ARGS(&this->m_device)));
		SUCCESS_LOG(L"Default D3D12 Device Created\n");
	}

	void GraphicsDevice::CreateD3D12Device(UINT8 deviceIndex)
	{
		auto adapter_list = this->GetGraphicsAdapterList();
		INFO_LOG(L"Creating D3D12 Device\n");
		DXCall(D3D12CreateDevice(adapter_list[deviceIndex].Get(), this->m_minimum_feature_level, IID_PPV_ARGS(&this->m_device)));
		SUCCESS_LOG(L"D3D12 Device Created\n");
	}

	std::vector<WRL::ComPtr<IDXGIAdapter>> GraphicsDevice::GetGraphicsAdapterList() const
	{
		INFO_LOG(L"Fetching Complete Adapter List\n");
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
		INFO_LOG(L"Finished Fetching Complete Adapter List\n");
		return adapter_list;
	}

	void GraphicsDevice::QueryAllDescriptorSizes()
	{
		INFO_LOG(L"Fetching Descriptor Handle Increment Sizes\n");
		const UINT64 RTV_size = this->QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		const UINT64 CBV_SRV_UAV_size = this->QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const UINT64 DSV_size = this->QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		const UINT64 Sampler_size = this->QueryDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		this->descriptorSizes = {RTV_size,CBV_SRV_UAV_size,DSV_size,Sampler_size};
		SUCCESS_LOG(L"Finished Fetching Descriptor Handle Increment Sizes\n");
	}

	UINT64 GraphicsDevice::QueryDescriptorSize(enum D3D12_DESCRIPTOR_HEAP_TYPE descriptorType) const
	{
		return this->m_device->GetDescriptorHandleIncrementSize(descriptorType);
	}

	inline void GraphicsDevice::CreateGraphicsCommandQueue()
	{
		this->m_graphics_command_queue = new GraphicsCommandQueue(*this);
	}

	void GraphicsDevice::CheckSupportedMSAALevels(DXGI_FORMAT backbufferFormat)
	{
		INFO_LOG(L"Fetching Supported MSAA Levels\n");
		this->supported_mssa_levels.clear();
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS quality_levels = {};
		quality_levels.Format = backbufferFormat;
		quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		quality_levels.SampleCount = 1;
		bool continueSupportCheck = true;
		while(continueSupportCheck)
		{
			DXCall(this->m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &quality_levels, sizeof(quality_levels)));
			if(quality_levels.NumQualityLevels > 0)
			{
				this->supported_mssa_levels.emplace_back(quality_levels.SampleCount);
				quality_levels.SampleCount *= 2;
			} else {
				continueSupportCheck = false;
			}
		}
		SUCCESS_LOG(L"Finished Fetching Supported MSAA Quality Levels\n");
	}

	CommandQueue* GraphicsDevice::GetGraphicsCommandQueue()
	{
		return this->m_graphics_command_queue;
	}
}
