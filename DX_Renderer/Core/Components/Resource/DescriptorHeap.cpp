#include "DescriptorHeap.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
	void DescriptorHeap::operator=(const DescriptorHeap& other)
	{
		INFO_LOG(L"Copying Descriptor Heap");
		this->ContainedDescriptorType = other.ContainedDescriptorType;
		this->DescriptorCount = other.DescriptorCount;
		this->m_descriptor_handle_increment_size = other.m_descriptor_handle_increment_size;
		this->m_descriptor_heap.Reset();
		DXCall(other.m_descriptor_heap.CopyTo(&this->m_descriptor_heap));
		SUCCESS_LOG(L"Finished Copying Descriptor Heap");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::operator[](const size_t index) const
	{
		if(index < this->DescriptorCount)
		{
			return {this->m_descriptor_heap->GetCPUDescriptorHandleForHeapStart().ptr + index * this->m_descriptor_handle_increment_size};
		}
		WARNING_LOG(L"Attempt To Access Out Of Bound Descriptor Handle From Descriptor Heap");
		return {NULL};
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::Get(const size_t index)
	{
		return {this->m_descriptor_heap->GetGPUDescriptorHandleForHeapStart().ptr + index * this->m_descriptor_handle_increment_size};
	}

	ID3D12DescriptorHeap* DescriptorHeap::GetRAWInterface() const
	{
		return this->m_descriptor_heap.Get();
	}

	DescriptorHeap::DescriptorHeap(GraphicsDevice& device, UINT8 descriptorCount, DescriptorType type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
		:ContainedDescriptorType(type), DescriptorCount(descriptorCount)
	{
		const DescriptorSizes descriptor_sizes = device.GetDescriptorSizes();
		switch(type)
		{
			case DescriptorType::RenderTargetView:
			{
				INFO_LOG(L"Creating Render Target View Descript Heap");
				this->m_descriptor_handle_increment_size = descriptor_sizes.RTV;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				break;
			}
			case DescriptorType::DepthStencilBuffer:
			{
				INFO_LOG(L"Creating Depth Stencil View Descriptor Heap");
				this->m_descriptor_handle_increment_size = descriptor_sizes.DSV;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				break;
			}
			case DescriptorType::ConstantBufferView:
			{
				INFO_LOG(L"Creating Constant Buffer View, Shader Resource View and Unordered Access View Descriptor Heap");
				this->m_descriptor_handle_increment_size = descriptor_sizes.CBV_SRV_UAV;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				break;
			}
			case DescriptorType::Sampler:
			{
				INFO_LOG(L"Creating Sampler Descripor Heap");
				this->m_descriptor_handle_increment_size = descriptor_sizes.Sampler;
				this->m_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				break;
			}
			default:
				break;
		}
		this->CreateDescriptorHeap(device, flags);
	}

	void DescriptorHeap::CreateDescriptorHeap(GraphicsDevice& device, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heap_description = {};
		heap_description.Flags = flags;
		heap_description.NodeMask = 0;
		heap_description.NumDescriptors = this->DescriptorCount;
		heap_description.Type = this->m_heap_type;

		INFO_LOG(L"Creating Descriptor Heap");
		DXCall(device->CreateDescriptorHeap(&heap_description, IID_PPV_ARGS(&this->m_descriptor_heap)));
		SUCCESS_LOG(L"Descriptor Heap Created");
	}
}
