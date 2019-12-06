#pragma once

#include <cmath>

#include "Resource.hpp"
#include "../GraphicsDevice.hpp"
#include "DescriptorHeap.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
	template<typename T>
	struct ConstantBuffer : public Resource
	{
	public:
		const UINT64 ConstantBufferSizeMultiplier = 256;
	private:
		DescriptorHeap m_descriptor_heap;
		std::vector<T> m_data;
	public:
		ConstantBuffer(GraphicsDevice& device, const std::vector<T> data):m_data(data)
		{
			this->m_descriptor_heap = device.CreateConstantBufferDescriptorHeap(1);
			
			this->m_resource_description = this->ConstantBuffer<T>::CreateResourceDescription();
			this->m_optimized_clear_value = {};
			this->m_resource_heap_description = this->ConstantBuffer<T>::CreateResourceHeapDescription();

			this->CreateConstantBufferResource(device);
			this->CreateConstantBufferDescriptor(device);
		}
		
	protected:
		void CreateConstantBufferDescriptor(GraphicsDevice& device)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constant_buffer_description = {};
			constant_buffer_description.BufferLocation = this->m_resource->GetGPUVirtualAddress();
			constant_buffer_description.SizeInBytes = this->CalculateBufferSize();
			
			device->CreateConstantBufferView(&constant_buffer_description, this->m_descriptor_heap[0]);
		}
		
		void CreateConstantBufferResource(GraphicsDevice& device)
		{
			INFO_LOG(L"Creating Constant Buffer GPU Resource");
			DXCall(device->CreateCommittedResource(
				&this->m_descriptor_heap,
				D3D12_HEAP_FLAG_NONE,
				&this->m_resource_description,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				IID_PPV_ARGS(&this->m_resource)));
			SUCCESS_LOG(L"Constant Buffer GPU Resource Created");
		}
		
		D3D12_RESOURCE_DESC CreateResourceDescription() override
		{
			D3D12_RESOURCE_DESC resource_description = {};
			resource_description.SampleDesc.Count = 1;
			resource_description.SampleDesc.Quality = 0;
			resource_description.Alignment = 0;
			resource_description.MipLevels = 1;
			resource_description.DepthOrArraySize = 1;
			resource_description.Width = this->CalculateBufferSize();
			resource_description.Height = 1;
			resource_description.Flags = D3D12_RESOURCE_FLAG_NONE;
			resource_description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resource_description.Format = DXGI_FORMAT_UNKNOWN;
			resource_description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			return resource_description;
		}

		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override
		{
			return {};
		}

		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override
		{
			D3D12_HEAP_PROPERTIES heap_properties = {};
			heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;
			heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heap_properties.CreationNodeMask = 1;
			heap_properties.VisibleNodeMask = 1;

			return heap_properties;
		}

		UINT64 CalculateBufferSize()
		{
			const UINT64 data_buffer_size = this->m_data.size() * sizeof(T);
			return ceil(data_buffer_size / ConstantBufferSizeMultiplier) * ConstantBufferSizeMultiplier;
		}
	};

}
