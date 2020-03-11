#include "DepthStencilBuffer.hpp"
#include "../../../Tooling/Validate.hpp"
#include "../GraphicsDevice.hpp"
#include "DescriptorHeap.hpp"
#include "../Command List/GraphicsCommandList.hpp"
#include "ResourceBarrier.hpp"

namespace DXR
{
	DepthStencilBuffer::DepthStencilBuffer(GraphicsDevice& device, GraphicsCommandList& commandList,
		DescriptorHeap& heap, size_t HeapIndex, Resolution& resolution): Resource(heap, HeapIndex)
	{
		this->m_resolution = resolution;
		D3D12_RESOURCE_DESC resource_description = this->DepthStencilBuffer::CreateResourceDescription();
		D3D12_CLEAR_VALUE optimized_clear_value = this->DepthStencilBuffer::CreateOptimizedClearValue();
		D3D12_HEAP_PROPERTIES heap_description = this->DepthStencilBuffer::CreateResourceHeapDescription();

		INFO_LOG(L"Creating Depth Stencil Buffer GPU Resource And Corresponding CPU Descriptor");
		DXCall(device->CreateCommittedResource(&heap_description,D3D12_HEAP_FLAG_NONE,&resource_description,D3D12_RESOURCE_STATE_COMMON,&optimized_clear_value,IID_PPV_ARGS(&this->m_resource)));
		device->CreateDepthStencilView(this->m_resource.Get(),nullptr,(*this->m_descriptor_heap)[0]);
		SUCCESS_LOG(L"Depth Stencil Buffer GPU Resource And CPU Descriptor Created");

		const ResourceBarrier resource_barrier = {*this->m_resource.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_DEPTH_WRITE};
		resource_barrier.ExecuteResourceBarrier(commandList);
		INFO_LOG(L"Queued Resource Barrier Into Command List For Execution");
	}

	void DepthStencilBuffer::Clear(GraphicsCommandList& commandList) const
	{
		commandList->ClearDepthStencilView((*this->m_descriptor_heap)[this->m_heap_index], D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}

	D3D12_RESOURCE_DESC DepthStencilBuffer::CreateResourceDescription()
	{
		D3D12_RESOURCE_DESC resource_description = {};
		resource_description.SampleDesc.Count = 1;
		resource_description.SampleDesc.Quality = 0;
		resource_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		resource_description.Alignment = 0;
		resource_description.DepthOrArraySize = 1;
		resource_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resource_description.MipLevels = 1;
		resource_description.Width = this->m_resolution.Width;
		resource_description.Height = this->m_resolution.Height;
		resource_description.Format = this->DepthStencilBufferFormat;
		resource_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		
		return resource_description;
	}

	D3D12_CLEAR_VALUE DepthStencilBuffer::CreateOptimizedClearValue()
	{
		D3D12_CLEAR_VALUE clear_value = {};
		clear_value.Format = this->DepthStencilBufferFormat;
		clear_value.DepthStencil.Depth = 1.0f;
		clear_value.DepthStencil.Stencil = 0;
		
		return clear_value;
	}

	D3D12_HEAP_PROPERTIES DepthStencilBuffer::CreateResourceHeapDescription()
	{
		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		
		return heap_properties;
	}
}
