#include "Texture.hpp"
#include "../GPU Buffers/GPUDefaultBuffer.hpp"
#include "../GPU Buffers/GPUUploadBuffer.hpp"

namespace DXR
{
	Texture::Texture(const std::wstring& filepath)
	{
		this->m_texture_data = TextureFS::LoadTextureData(filepath);
	}

	DXGI_FORMAT Texture::DetermineTextureDataFormat()
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
}
