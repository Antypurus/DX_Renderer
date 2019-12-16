#include "GPUDefaultBuffer.hpp"
#include "../../GraphicsDevice.hpp"
#include "../../Command List/GraphicsCommandList.hpp"
#include "../../../../Tooling/Validate.hpp"
#include "../ResourceBarrier.hpp"

namespace DXR
{
	GPUDefaultBuffer::GPUDefaultBuffer(GraphicsDevice& device, GraphicsCommandList& commandList, UINT64 elementCount, UINT64 elementSize)
		:m_element_count(elementCount),m_element_size(elementSize)
	{
		this->m_resource_description = this->GPUDefaultBuffer::CreateResourceDescription();
		this->m_optimized_clear_value = {};
		this->m_resource_heap_description = this->GPUDefaultBuffer::CreateResourceHeapDescription();

		this->CreateResource(device,commandList);
	}

	void GPUDefaultBuffer::CreateResource(GraphicsDevice& device, GraphicsCommandList& commandList)
	{
		INFO_LOG(L"Creating GPU Buffer Resource");
		DXCall(device->CreateCommittedResource(
			&this->m_resource_heap_description,
			D3D12_HEAP_FLAG_NONE,
			&this->m_resource_description,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&this->m_resource)));
		SUCCESS_LOG(L"GPU Buffer Resource Created");

		const ResourceBarrier resource_barrier = {
			*this->m_resource.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST};
		resource_barrier.ExecuteResourceBarrier(commandList);
		INFO_LOG(L"Queued GPU Resource State Transition From Common To Generic Read");
	}

	D3D12_RESOURCE_DESC GPUDefaultBuffer::CreateResourceDescription()
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

	D3D12_CLEAR_VALUE GPUDefaultBuffer::CreateOptimizedClearValue()
	{
		return {};
	}

	D3D12_HEAP_PROPERTIES GPUDefaultBuffer::CreateResourceHeapDescription()
	{
		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
		return heap_properties;
	}
}
