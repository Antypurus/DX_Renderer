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

	size_t IndexBuffer::GetIndexCount() const
	{
		return this->m_indices.size();
	}

	void IndexBuffer::CreateIndexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList)
	{
		this->m_index_buffer = std::make_unique<GPUDefaultBuffer>(device, commandList, this->m_indices.size(), sizeof(UINT));
		this->m_index_buffer->GetResource()->SetName(L"Index Buffer");
	}

	void IndexBuffer::CreateUploadBuffer(GraphicsDevice& device)
	{
		const size_t entry_count = this->m_indices.size();
		auto data = new UINT[entry_count];
		for(size_t i = 0;i < entry_count;++i)
		{
			data[i] = this->m_indices[i];
		}
		this->m_upload_buffer = std::make_unique<GPUUploadBuffer>(device, this->m_indices.size(), sizeof(UINT), data);
		delete[] data;

		this->m_upload_buffer->GetResource()->SetName(L"Index Buffer Upload Buffer");
	}

	void IndexBuffer::UploadData(GraphicsCommandList& commandList) const
	{
		this->m_upload_buffer->CopyDataToGPUBuffer(commandList, *this->m_index_buffer);
	}

	void IndexBuffer::CreateIndexBufferDescriptor()
	{
		D3D12_INDEX_BUFFER_VIEW index_buffer_descriptor = {};
		index_buffer_descriptor.BufferLocation = (*this->m_index_buffer)->GetGPUVirtualAddress();
		index_buffer_descriptor.Format = DXGI_FORMAT_R32_UINT;
		index_buffer_descriptor.SizeInBytes = (UINT)(this->m_indices.size() * sizeof(UINT));

		this->m_index_buffer_descriptor = index_buffer_descriptor;
	}
}
