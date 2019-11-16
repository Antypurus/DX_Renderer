#pragma once

#include <wrl.h>
#include <d3d12.h>

namespace DXR
{

	using namespace Microsoft;

	struct GraphicsDevice;

	enum class DescriptorType
	{
		RenderTargetView,
		DepthStencilBuffer,
		ConstantBufferView,
		Sampler,
		None
	};
	
	struct DescriptorHeap
	{
	public:
		friend GraphicsDevice;
	private:
		DescriptorType ContainedDescriptorType = DescriptorType::None;
		UINT DescriptorCount = 0;
		UINT64 m_descriptor_handle_increment_size = 0;
		D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type;
		WRL::ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
	public:
		DescriptorHeap(){}
		void operator=(const DescriptorHeap& other);
		D3D12_CPU_DESCRIPTOR_HANDLE operator[](const size_t index) const;
	private:
		DescriptorHeap(GraphicsDevice& device, UINT8 descriptorCount, DescriptorType type);
		inline void CreateDescriptorHeap(GraphicsDevice& device);
	};

}