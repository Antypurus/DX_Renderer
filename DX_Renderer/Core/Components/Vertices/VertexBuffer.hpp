#pragma once
#include "../Command List/GraphicsCommandList.hpp"
#include "../GraphicsDevice.hpp"
#include "../Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "../Resource/GPU Buffers/GPUUploadBuffer.hpp"
#include "Vertex.hpp"
#include <vector>
#include <memory>

namespace DXR
{
	template<class VertexStruct>
	struct VertexBuffer
	{
	public:
	private:
		std::unique_ptr<GPUDefaultBuffer> m_vertex_buffer;
		std::unique_ptr<GPUUploadBuffer> m_upload_buffer;
		std::vector<VertexStruct> m_vertices;
		D3D12_VERTEX_BUFFER_VIEW m_vertex_buffer_descriptor;
	public:
		VertexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList, const std::vector<VertexStruct>& vertices) :m_vertices(vertices)
		{
			static_assert(std::is_base_of<Vertex, VertexStruct>::value, "Provided Struct Must Be Derived From Vertex");

			INFO_LOG(L"Creating Vertex Buffer");
			this->CreateVertexBuffer(device, commandList);
			this->CreateUploadBuffer(device);
			this->UploadData(commandList);
			this->CreateVertexBufferDescriptor();
			SUCCESS_LOG(L"Vertex Buffer Created");
		}

		struct D3D12_VERTEX_BUFFER_VIEW GetVertexBufferDescriptor() const
		{
			return this->m_vertex_buffer_descriptor;
		}
	private:
		void CreateUploadBuffer(GraphicsDevice& device)
		{
			auto vertices = new VertexStruct[this->m_vertices.size()];
			for(size_t i = 0;i < this->m_vertices.size();++i)
			{
				vertices[i] = this->m_vertices[i];
			}
			this->m_upload_buffer = std::make_unique<GPUUploadBuffer>(device, this->m_vertices.size(), this->m_vertices[0].GetElementSize(), vertices);
			delete[] vertices;
		}

		void CreateVertexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList)
		{
			this->m_vertex_buffer = std::make_unique<GPUDefaultBuffer>(device, commandList, this->m_vertices.size(), this->m_vertices[0].GetElementSize());
		}

		void UploadData(GraphicsCommandList& commandList)
		{
			this->m_upload_buffer->CopyDataToGPUBuffer(commandList, *this->m_vertex_buffer);
		}

		void CreateVertexBufferDescriptor()
		{
			D3D12_VERTEX_BUFFER_VIEW vertex_buffer_descriptor = {};
			vertex_buffer_descriptor.BufferLocation = (*this->m_vertex_buffer)->GetGPUVirtualAddress();
			vertex_buffer_descriptor.SizeInBytes = (UINT)this->m_vertices.size() * this->m_vertices[0].GetElementSize();
			vertex_buffer_descriptor.StrideInBytes = this->m_vertices[0].GetElementSize();

			this->m_vertex_buffer_descriptor = vertex_buffer_descriptor;
		}
	};
}
