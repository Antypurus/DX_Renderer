#pragma once
#include "Resource.hpp"

namespace DXR
{

	struct ConstantBuffer : public Resource
	{
	public:
	private:
	public:
	protected:
		D3D12_RESOURCE_DESC CreateResourceDescription() override;
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override;
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override;
	};

}
