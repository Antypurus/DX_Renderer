#include "DescriptorHeap.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
	void DescriptorHeap::operator=(const DescriptorHeap& other)
	{
		this->ContainedDescriptorType = other.ContainedDescriptorType;
		this->DescriptorCount = other.DescriptorCount;
		this->m_descriptor_handle_increment_size = other.m_descriptor_handle_increment_size;
		this->m_descriptor_heap.Reset();
		other.m_descriptor_heap.CopyTo(&this->m_descriptor_heap);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::operator[](const size_t index) const
	{
		if(index < this->DescriptorCount)
		{
			return {this->m_descriptor_heap->GetCPUDescriptorHandleForHeapStart().ptr + index * this->m_descriptor_handle_increment_size};
		}
		return {NULL};
	}

	DescriptorHeap::DescriptorHeap(GraphicsDevice& device, UINT8 descriptorCount, DescriptorType type)
		:ContainedDescriptorType(type), DescriptorCount(descriptorCount)
	{
		const DescriptorSizes descriptor_sizes = device.GetDescriptorSizes();
		switch(type)
		{
			case DescriptorType::RenderTargetView:
			{
				this->m_descriptor_handle_increment_size = descriptor_sizes.RTV;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				break;
			}
			case DescriptorType::DepthStencilBuffer:
			{
				this->m_descriptor_handle_increment_size = descriptor_sizes.DSV;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				break;
			}
			case DescriptorType::ConstantBufferView:
			{
				this->m_descriptor_handle_increment_size = descriptor_sizes.CBV_SRV_UAV;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				break;
			}
			case DescriptorType::Sampler:
			{
				this->m_descriptor_handle_increment_size = descriptor_sizes.Sampler;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				break;
			}
			default:
				break;
		}
		this->CreateDescriptorHeap(device);
	}

	void DescriptorHeap::CreateDescriptorHeap(GraphicsDevice& device)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heap_description = {};
		heap_description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heap_description.NodeMask = 0;
		heap_description.NumDescriptors = this->DescriptorCount;
		heap_description.Type = this->m_heap_type;

		DXCall(device->CreateDescriptorHeap(&heap_description, IID_PPV_ARGS(&this->m_descriptor_heap)));
	}
}
