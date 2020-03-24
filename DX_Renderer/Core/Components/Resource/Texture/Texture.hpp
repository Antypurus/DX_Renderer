#pragma once
#include "../../../Windows Abstractions/TextureFS.hpp"

namespace  DXR
{
	struct GPUDefaultBuffer;
	struct GPUUploadBuffer;

	struct Texture
	{
	public:
	private:
		TextureData m_texture_data;
		DXGI_FORMAT m_texture_format;
		std::unique_ptr<GPUUploadBuffer> m_upload_buffer = nullptr;
		std::unique_ptr<GPUDefaultBuffer> m_texture_buffer = nullptr;
	public:
		Texture(const std::wstring& filepath);
	private:
		DXGI_FORMAT DetermineTextureDataFormat();
	};

}
