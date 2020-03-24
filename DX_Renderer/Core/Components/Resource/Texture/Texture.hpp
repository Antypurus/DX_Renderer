#pragma once
#include <d3d12.h>

#include "../../../Windows Abstractions/TextureFS.hpp"

namespace  DXR
{
	struct GraphicsCommandList;
	struct GraphicsDevice;
	struct GPUDefaultBuffer;
	struct GPUUploadBuffer;

	struct Texture
	{
	public:
	private:
		TextureData m_texture_data;
		DXGI_FORMAT m_texture_format;
		D3D12_RESOURCE_DESC m_resource_description = {};
		std::unique_ptr<GPUUploadBuffer> m_upload_buffer = nullptr;
		std::unique_ptr<GPUDefaultBuffer> m_texture_buffer = nullptr;
	public:
		Texture(const std::wstring& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList);
	private:
		DXGI_FORMAT DetermineTextureDataFormat() const;
		void CreateResourceDescription();
		void UploadTextureData(GraphicsCommandList& commandList);
	};

}
