#include "IndexBuffer.hpp"
#include "../Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "../Resource/GPU Buffers/GPUUploadBuffer.hpp"
#include "../GraphicsDevice.hpp"
#include "../Command List/GraphicsCommandList.hpp"

namespace DXR
{
	IndexBuffer::IndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList,
							 const std::vector<UINT>& indices) :m_indices(indices)
	{
		this->CreateIndexBuffer(device, commandList);
		this->CreateUploadBuffer(device);
		this->UploadData(commandList);
		this->CreateIndexBufferDescriptor();
	}

	D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetIndexBufferDescriptor() const
	{
		return this->m_index_buffer_descriptor;
	}

	void IndexBuffer::CreateIndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList)
	{
		this->m_index_buffer = std::make_unique<GPUDefaultBuffer>(device, commandList, this->m_indices.size(), sizeof(UINT));
	}

	void IndexBuffer::CreateUploadBuffer(GraphicsDevice& device)
	{
		auto data = new UINT[this->m_indices.size()];
		for(size_t i = 0;i < this->m_indices.size();++i)
		{
			data[i] = this->m_indices[i];
		}
		this->m_upload_buffer = std::make_unique<GPUUploadBuffer>(device, this->m_indices.size(), sizeof(UINT), data);
		delete[] data;
	}

	void IndexBuffer::UploadData(GraphicsCommandList& commandList)
	{
		this->m_upload_buffer->CopyDataToGPUBuffer(commandList, *this->m_index_buffer);
	}

	void IndexBuffer::CreateIndexBufferDescriptor()
	{
		D3D12_INDEX_BUFFER_VIEW index_buffer_descriptor = {};
		index_buffer_descriptor.BufferLocation = (*this->m_index_buffer)->GetGPUVirtualAddress();
		index_buffer_descriptor.Format = DXGI_FORMAT_R16_UINT;
		index_buffer_descriptor.SizeInBytes = this->m_indices.size() * sizeof(UINT);

		this->m_index_buffer_descriptor = index_buffer_descriptor;
	}
}
