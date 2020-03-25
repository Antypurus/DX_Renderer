#include "Texture.hpp"

#include <thread>

#include "../GPU Buffers/GPUDefaultBuffer.hpp"
#include "../../GraphicsDevice.hpp"
#include "../../Command List/GraphicsCommandList.hpp"

namespace DXR
{
	
	Texture::Texture(const std::wstring& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		this->m_texture_data = TextureFS::LoadTextureData(filepath);
		this->m_texture_format = this->DetermineTextureDataFormat();
		this->CreateResourceDescription();

		this->CreateTextureBuffers(Device,CommandList);
	}

	void Texture::CreateTextureBuffers(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		this->m_upload_buffer = std::make_unique<TextureUploadBuffer>(Device, this->m_texture_data.GetTextureSize(), (void*)this->m_texture_data.GetTextureData());
		this->m_texture_buffer = std::make_unique<GPUDefaultBuffer>(Device, CommandList, 1, CalculateBufferSize(this->m_texture_data), this->m_resource_description);

		this->m_upload_buffer->UploadTextureDataToDefaultBuffer(CommandList, *this->m_texture_buffer, this->m_resource_description, this->m_texture_data.CalculateRowPitch());
	}

	DXGI_FORMAT Texture::DetermineTextureDataFormat() const
	{
		if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		return DXGI_FORMAT_UNKNOWN;
	}

	void Texture::CreateResourceDescription()
	{
		D3D12_RESOURCE_DESC resource = {};
		resource.Alignment = 0;
		resource.DepthOrArraySize = 1;//For now only 2D textures
		resource.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resource.Flags = D3D12_RESOURCE_FLAG_NONE;
		resource.Format = this->m_texture_format;
		resource.Height = this->m_texture_data.GetHeight();
		resource.Width = this->m_texture_data.GetWidth();
		resource.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resource.MipLevels = 1;
		// For now no texture multisampling is allowed
		resource.SampleDesc.Count = 1;
		resource.SampleDesc.Quality = 0;

		this->m_resource_description = resource;
	}

	void Texture::UploadTextureData(GraphicsCommandList& CommandList)
	{
		this->m_upload_buffer->CopyDataToGPUBuffer(CommandList, *this->m_texture_buffer);
		//NOTE(Tiago): Evict Upload Bufffer?
	}

	UINT64 Texture::CalculateBufferSize(TextureData& data)
	{
		int textureHeapSize = ((((data.GetWidth() * data.GetBytesPerPixel()) + 255) & ~255) * (data.GetHeight() - 1)) + (data.GetWidth() * data.GetBytesPerPixel());
		return textureHeapSize;
	}
}
