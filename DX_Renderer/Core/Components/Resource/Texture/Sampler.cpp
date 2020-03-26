#include "Sampler.h"
#include "../../GraphicsDevice.hpp"

namespace DXR
{
	SamplerSettings::SamplerSettings(FLOAT BorderColor[4], Filter filter, UINT AnisotropicLevels,UINT MipmapLevelCount)
	{
		TextureAdressMode = AddressMode::Border;
		TextureFilter = filter;
		this->AnisotropicLevels = AnisotropicLevels;
		this->MipmapLevelCount = MipmapLevelCount;
		memcpy(this->BorderColor, BorderColor, sizeof(FLOAT) * 4);
	}

	SamplerSettings::SamplerSettings(AddressMode addressMode, Filter filter, UINT AnisotropicLevels,UINT MipmapLevelCount)
	{
		TextureAdressMode = addressMode;
		TextureFilter = filter;
		this->MipmapLevelCount = MipmapLevelCount;
		this->AnisotropicLevels = AnisotropicLevels;
	}

	D3D12_FILTER SamplerSettings::GetFilterType() const
	{
		switch (this->TextureFilter)
		{
			case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
			case Filter::Linear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			case Filter::Point: return D3D12_FILTER_MIN_MAG_MIP_POINT;
			default: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;//Linear is the default filter
		}
	}

	D3D12_TEXTURE_ADDRESS_MODE SamplerSettings::GetAddressMode() const
	{
		switch(this->TextureAdressMode)
		{
			case AddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			case AddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
			case AddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			case AddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			default: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;// default address mode is wrap
		}
	}

	Sampler::Sampler(GraphicsDevice& Device, const SamplerSettings& settings)
	{
		this->CreateDescriptorHeap(Device);
		this->CreateSampler(Device,settings);
	}

	void Sampler::CreateSampler(GraphicsDevice& device, const SamplerSettings& settings) const
	{
		D3D12_SAMPLER_DESC sampler_desc = {};
		sampler_desc.AddressU = settings.GetAddressMode();
		sampler_desc.AddressV = settings.GetAddressMode();
		sampler_desc.AddressW = settings.GetAddressMode();
		if(settings.TextureAdressMode == AddressMode::Border)
		{
			memcpy(sampler_desc.BorderColor,settings.BorderColor,sizeof(FLOAT)*4);
		}
		sampler_desc.Filter = settings.GetFilterType();
		sampler_desc.MaxAnisotropy = settings.AnisotropicLevels;
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = settings.MipmapLevelCount - 1;

		device->CreateSampler(&sampler_desc, this->m_heap[0]);
	}

	void Sampler::CreateDescriptorHeap(GraphicsDevice& Device)
	{
		this->m_heap = Device.CreateSamplerDescriptorHeap(1);;
		this->m_heap_index = 0;
	}
}
