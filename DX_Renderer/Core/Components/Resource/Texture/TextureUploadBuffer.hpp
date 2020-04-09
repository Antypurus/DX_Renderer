#pragma once
#include "../GPU Buffers/GPUUploadBuffer.hpp"
#include "../../../Windows Abstractions/TextureFS.hpp"

namespace DXR
{

	struct TextureUploadBuffer: public GPUUploadBuffer
	{
	public:
	private:
	public:
		TextureUploadBuffer(GraphicsDevice& Device, UINT64 elementSize, TextureData& data);
		void UploadTextureDataToDefaultBuffer(GraphicsCommandList& commandList, GPUDefaultBuffer& buffer, D3D12_RESOURCE_DESC& ResourceDescription, UINT64 RowPitch) const;
	private:
		void UploadTextureData(TextureData& data);
	};

}
