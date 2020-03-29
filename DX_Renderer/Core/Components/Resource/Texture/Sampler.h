#pragma once
#include "../DescriptorHeap.hpp"
#include "../Resource.hpp"

namespace DXR
{

	enum class AddressMode
	{
		Wrap,
		Mirror,
		Border,
		Clamp
	};

	enum class Filter
	{
		Point,
		Linear,
		Anisotropic
	};

	struct SamplerSettings
	{
	public:
		UINT MipmapLevelCount = 1;
		AddressMode TextureAdressMode;
		Filter TextureFilter;
		UINT AnisotropicLevels;
		FLOAT BorderColor[4];
	public:
		SamplerSettings(FLOAT BorderColor[4], Filter filter, UINT AnisotropicLevels = 0,UINT MipmapLevelCount = 1);
		SamplerSettings(AddressMode addressMode, Filter filter, UINT AnisotropicLevels = 0,UINT MipmapLevelCount = 1);
		[[nodiscard]] D3D12_FILTER GetFilterType() const;
		[[nodiscard]] D3D12_TEXTURE_ADDRESS_MODE GetAddressMode() const;
	};

	struct Sampler :public Resource
	{
	public:
	private:
		DescriptorHeap m_heap;
	public:
		Sampler() = default;
		Sampler(GraphicsDevice& Device, const SamplerSettings& settings = {AddressMode::Wrap,Filter::Anisotropic,16});
		void operator=(const Sampler& sampler);
	private:
		void CreateSampler(GraphicsDevice& device, const SamplerSettings& settings) const;
		void CreateDescriptorHeap(GraphicsDevice& Device);
	protected:
		D3D12_RESOURCE_DESC CreateResourceDescription(D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE) override {return {};}
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override {return {};}
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override {return {};}
	};

}
