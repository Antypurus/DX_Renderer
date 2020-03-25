#include "TextureUploadBuffer.hpp"
#include "../../Command List/GraphicsCommandList.hpp"
#include <cmath>

namespace DXR
{

	TextureUploadBuffer::TextureUploadBuffer(GraphicsDevice& Device, UINT64 elementSize,
		void* Data) :GPUUploadBuffer(Device, 1, elementSize, Data)
	{
	}

	void TextureUploadBuffer::UploadTextureDataToDefaultBuffer(GraphicsCommandList& commandList, GPUDefaultBuffer& buffer, D3D12_RESOURCE_DESC& ResourceDescription, UINT64 RowPitch) const
	{
		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		source.pResource = this->m_resource.Get();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footprint = &source.PlacedFootprint;
		footprint->Footprint.Width = ResourceDescription.Width;
		footprint->Footprint.Height = ResourceDescription.Height;
		footprint->Footprint.Depth = ResourceDescription.DepthOrArraySize;
		footprint->Footprint.Format = ResourceDescription.Format;
		footprint->Footprint.RowPitch = RowPitch;

		D3D12_TEXTURE_COPY_LOCATION destination = {};
		destination.pResource = buffer.GetResource();
		destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destination.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
	}
}

