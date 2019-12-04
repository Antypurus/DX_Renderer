#include "GPUUploadBuffer.hpp"
#include "../../GraphicsDevice.hpp"
#include "../../../../Tooling/Validate.hpp"
#include "../../Command List/GraphicsCommandList.hpp"
#include "GPUDefaultBuffer.hpp"
#include <d3d12.h>

namespace DXR
{
	GPUUploadBuffer::GPUUploadBuffer(GraphicsDevice& device, UINT64 elementCount, UINT64 elementSize, void* Data)
		:m_element_count(elementCount), m_element_size(elementSize)
	{
		this->m_resource_description = this->GPUUploadBuffer::CreateResourceDescription();
		this->m_optimized_clear_value = {};
		this->m_resource_heap_description = this->GPUUploadBuffer::CreateResourceHeapDescription();

		this->CreateResource(device);
		this->UploadDataFromCPUBuffer(Data);
	}

	void GPUUploadBuffer::CopyDataToGPUBuffer(GraphicsCommandList& commandList, GPUDefaultBuffer& buffer)
	{
		commandList->CopyBufferRegion(buffer.GetResource(), 0, this->m_resource.Get(), 0, this->m_element_count * this->m_element_size);
		INFO_LOG(L"Queued Data Transfer From GPU Upload Buffer To GPU Default Buffer");
	}

	void GPUUploadBuffer::UploadDataFromCPUBuffer(void* Data) const
	{
		INFO_LOG(L"Started Uploading CPU Data To GPU Upload Buffer");

		D3D12_RANGE read_range = {};
		read_range.Begin = 0;
		read_range.End = ((size_t)((this->m_element_size) * this->m_element_count));

		void* proxy_data = nullptr;

		INFO_LOG(L"Mapping GPU Upload Buffer Data Address To CPU Data Address");
		DXCall(this->m_resource->Map(0, &read_range, &proxy_data));
		SUCCESS_LOG(L"GPU Upload Buffer Data Address Mapped To CPU Data Address");

		INFO_LOG(L"Writting Data To GPU Upload Buffer");
		memmove(proxy_data, Data, this->m_element_count * this->m_element_size);
		SUCCESS_LOG(L"GPU Upload Buffer Data Written");

		this->m_resource->Unmap(0, &read_range);
		INFO_LOG(L"Unmapped GPU Upload Buffer Data Adress From CPU Data Address");

		SUCCESS_LOG(L"CPU Data Uploaded To GPU Upload Buffer");
	}

	void GPUUploadBuffer::CreateResource(GraphicsDevice& device)
	{
		INFO_LOG(L"Creating GPU Upload Buffer Resource");
		DXCall(device->CreateCommittedResource(
			&this->m_resource_heap_description,
			D3D12_HEAP_FLAG_NONE,
			&this->m_resource_description,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&this->m_resource)));
		SUCCESS_LOG(L"GPU Upload Buffer Resource Created");
	}

	D3D12_RESOURCE_DESC GPUUploadBuffer::CreateResourceDescription()
	{
		D3D12_RESOURCE_DESC resource_description = {};
		resource_description.SampleDesc.Count = 1;
		resource_description.SampleDesc.Quality = 0;
		resource_description.Alignment = 0;
		resource_description.MipLevels = 1;
		resource_description.DepthOrArraySize = 1;
		resource_description.Width = this->m_element_count * this->m_element_size;
		resource_description.Height = 1;
		resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;
		resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_description.Format = DXGI_FORMAT_UNKNOWN;
		resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		return resource_description;
	}

	D3D12_CLEAR_VALUE GPUUploadBuffer::CreateOptimizedClearValue()
	{
		return {};
	}

	D3D12_HEAP_PROPERTIES GPUUploadBuffer::CreateResourceHeapDescription()
	{
		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
		return heap_properties;
	}
}
