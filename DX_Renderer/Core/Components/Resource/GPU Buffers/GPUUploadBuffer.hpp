#pragma once
#include "../Resource.hpp"

namespace DXR
{
	struct GPUDefaultBuffer;
	struct GraphicsCommandList;
	struct GraphicsDevice;

	class GPUUploadBuffer :public Resource
	{
	public:
	private:
		UINT64 m_element_count = 0;
		UINT64 m_element_size = 0;
	public:
		GPUUploadBuffer(GraphicsDevice& device, UINT64 elementCount, UINT64 elementSize, void* Data);
		void CopyDataToGPUBuffer(GraphicsCommandList& commandList, GPUDefaultBuffer& buffer);
		void UploadDataFromCPUBuffer(void* Data) const;
	protected:
		void CreateResource(GraphicsDevice& device);
		D3D12_RESOURCE_DESC CreateResourceDescription() override;
		D3D12_CLEAR_VALUE CreateOptimizedClearValue() override;
		D3D12_HEAP_PROPERTIES CreateResourceHeapDescription() override;
	};
}
