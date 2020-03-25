#include "TextureUploadBuffer.hpp"
#include "../../Command List/GraphicsCommandList.hpp"

namespace DXR
{

	TextureUploadBuffer::TextureUploadBuffer(GraphicsDevice& Device, UINT64 elementSize,
		void* Data):GPUUploadBuffer(Device,1,elementSize,Data)
	{
	}

	void TextureUploadBuffer::UploadTextureDataToDefaultBuffer(GraphicsCommandList& commandList, GPUDefaultBuffer& buffer, D3D12_RESOURCE_DESC& ResourceDescription) const
	{
		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		source.pResource = this->m_resource.Get();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint = source.PlacedFootprint;
		footprint.Offset = 0;
		footprint.Footprint.Width = ResourceDescription.Width;
		footprint.Footprint.Height = ResourceDescription.Height;
		footprint.Footprint.Depth = ResourceDescription.DepthOrArraySize;
		footprint.Footprint.Format = ResourceDescription.Format;
		footprint.Footprint.RowPitch = this->m_element_size;

		D3D12_TEXTURE_COPY_LOCATION destination = {};
		
		//commandList->CopyTextureRegion()
	}
}

