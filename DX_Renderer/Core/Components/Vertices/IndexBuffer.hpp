#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>

namespace DXR
{
	struct GraphicsCommandList;
	struct GraphicsDevice;
	struct GPUDefaultBuffer;
	struct GPUUploadBuffer;
    
	struct IndexBuffer
	{
        public:
		const static DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
        private:
		std::unique_ptr<GPUUploadBuffer> m_upload_buffer;
		std::unique_ptr<GPUDefaultBuffer> m_index_buffer;
		std::vector<UINT> m_indices;
		D3D12_INDEX_BUFFER_VIEW m_index_buffer_descriptor;
        public:
        IndexBuffer() = default;
		IndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList, const std::vector<UINT>& indices);
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferDescriptor() const;
		[[nodiscard]]size_t GetIndexCount() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetIndexBufferGPUAddress() const;
        private:
		void CreateIndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList);
		void CreateUploadBuffer(GraphicsDevice& device);
		void UploadData(GraphicsCommandList& commandList) const;
		void CreateIndexBufferDescriptor();
	};
}
