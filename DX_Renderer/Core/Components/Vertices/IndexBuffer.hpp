﻿#pragma once
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
	private:
		std::unique_ptr<GPUUploadBuffer> m_upload_buffer;
		std::unique_ptr<GPUDefaultBuffer> m_index_buffer;
		std::vector<uint32_t> m_indices;
		D3D12_INDEX_BUFFER_VIEW m_index_buffer_descriptor;
	public:
		IndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList, const std::vector<uint32_t>& indices);
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferDescriptor() const;
	private:
		void CreateIndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList);
		void CreateUploadBuffer(GraphicsDevice& device);
		void UploadData(GraphicsCommandList& commandList);
		void CreateIndexBufferDescriptor();
	};
}