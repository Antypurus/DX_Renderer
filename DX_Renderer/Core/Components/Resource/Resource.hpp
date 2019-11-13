#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace DXR
{
	using namespace Microsoft;

	struct DescriptorHeap;

	struct Resource
	{
	public:
	protected:
		WRL::ComPtr<ID3D12Resource> m_resource;
		DescriptorHeap* m_descriptor_heap = nullptr;// Non owning pointer to resource descriptor heap
		D3D12_RESOURCE_DESC m_resource_description = {};
		D3D12_CLEAR_VALUE m_optimized_clear_value = {};
		D3D12_HEAP_PROPERTIES m_resource_heap_description = {};
	public:
		ID3D12Resource* operator->() const;
	protected:
		Resource(DescriptorHeap& heap);
		virtual D3D12_RESOURCE_DESC CreateResourceDescription() = 0;
		virtual D3D12_CLEAR_VALUE CreateOptimizedClearValue() = 0;
		virtual D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() = 0;
	};

};