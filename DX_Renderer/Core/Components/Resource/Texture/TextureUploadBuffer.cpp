#include "TextureUploadBuffer.hpp"
#include "../../Command List/GraphicsCommandList.hpp"
#include <cmath>

namespace DXR
{

	TextureUploadBuffer::TextureUploadBuffer(GraphicsDevice& Device, UINT64 elementSize,
		TextureData& data)
	{
		this->m_element_count = 1;
		this->m_element_size = elementSize;

		this->m_resource_description = this->GPUUploadBuffer::CreateResourceDescription();
		this->m_optimized_clear_value = {};
		this->m_resource_heap_description = this->GPUUploadBuffer::CreateResourceHeapDescription();

		this->CreateResource(Device);
		this->UploadTextureData(data);
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

	void TextureUploadBuffer::UploadTextureData(TextureData& data)
	{
		INFO_LOG(L"Started Uploading CPU Data To GPU Upload Buffer");

		D3D12_RANGE read_range = {};
		read_range.Begin = 0;
		read_range.End = 0;

		char* proxy_data = nullptr;

		INFO_LOG(L"Mapping GPU Upload Buffer Data Address To CPU Data Address");
		DXCall(this->m_resource->Map(0, &read_range, (void**)&proxy_data));
		SUCCESS_LOG(L"GPU Upload Buffer Data Address Mapped To CPU Data Address");

		INFO_LOG(L"Writting Data To GPU Upload Buffer");

		for (size_t row = 0; row < data.GetHeight(); ++row)
		{
			memcpy(proxy_data + row * data.CalculateAlignedRowPitch(), data.GetTextureData() + row * data.CalculateRowPitch(), data.CalculateRowPitch());
		}

		SUCCESS_LOG(L"GPU Upload Buffer Data Written");

		this->m_resource->Unmap(0, &read_range);
		INFO_LOG(L"Unmapped GPU Upload Buffer Data Adress From CPU Data Address");

		SUCCESS_LOG(L"CPU Data Uploaded To GPU Upload Buffer");
	}
}

