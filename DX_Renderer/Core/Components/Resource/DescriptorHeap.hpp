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
		Sampler
	};
	
	struct DescriptorHeap
	{
	public:
		const DescriptorType DescriptorType;
	private:
		WRL::ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
	private:
		void CreateDescriptorHeap(GraphicsDevice& device);
	};

}