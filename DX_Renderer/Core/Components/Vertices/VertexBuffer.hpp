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
	public:
		VertexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList, std::vector<VertexStruct> vertices) :m_vertices(vertices)
		{
			static_assert(std::is_base_of<Vertex, VertexStruct>::value, "Provided Struct Must Be Derived From Vertex");

			this->CreateVertexBuffer(device, commandList);
			this->CreateUploadBuffer(device);
			this->UploadData(commandList);
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
			this->m_upload_buffer->CopyDataToGPUBuffer(commandList,*this->m_vertex_buffer);
		}
	};
}
