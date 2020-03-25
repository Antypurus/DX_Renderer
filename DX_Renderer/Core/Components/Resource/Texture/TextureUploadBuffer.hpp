#pragma once
#include "../GPU Buffers/GPUUploadBuffer.hpp"

namespace DXR
{

	struct TextureUploadBuffer:public GPUUploadBuffer
	{
	public:
	private:
	public:
		TextureUploadBuffer(GraphicsDevice& Device, UINT64 elementSize, void* Data);
	private:
		void UploadTextureDataToDefaultBuffer(GraphicsCommandList& commandList, GPUDefaultBuffer& buffer, D3D12_RESOURCE_DESC& ResourceDescription) const;
	};

}
