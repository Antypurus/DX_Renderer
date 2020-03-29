#pragma once
#include <d3d12.h>
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
		D3D12_INPUT_LAYOUT_DESC m_input_layout;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements;
	public:
		VertexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList, const std::vector<VertexStruct>& vertices) :m_vertices(vertices)
		{
			static_assert(std::is_base_of<Vertex, VertexStruct>::value, "Provided Struct Must Be Derived From Vertex");

			this->GenerateInputLayoutDescription();

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

		D3D12_INPUT_LAYOUT_DESC GetInputLayout()
		{
			return this->m_input_layout;
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetVertexBufferGPUAddress() const
		{
			return this->m_vertex_buffer->GetGPUAddress();
		}

		UINT64 GetVertexStride() const
		{
			return this->m_vertices[0].GetElementSize();
		}

		UINT GetVertexCount() const
		{
			return this->m_vertices.size();
		}

		DXGI_FORMAT GetVertexPositionFormat() const
		{
			//Note(Tiago): Position is assumed to be the 0-th element, however we might need to do a search for it in practice
			return this->m_input_elements[0].Format;
		}

	private:

		void GenerateInputLayoutDescription()
		{
			this->m_input_elements = this->m_vertices[0].GenerateInputElementDescription();

			D3D12_INPUT_LAYOUT_DESC input_layout = {};
			input_layout.NumElements = (UINT)this->m_input_elements.size();
			input_layout.pInputElementDescs = this->m_input_elements.data();

			this->m_input_layout = input_layout;
		}

		void CreateUploadBuffer(GraphicsDevice& device)
		{
			UINT8* vertices = new UINT8[this->m_vertices.size() * m_vertices[0].GetElementSize()];
			for (size_t i = 0; i < this->m_vertices.size(); ++i)
			{
				memcpy(&vertices[i * m_vertices[i].GetElementSize()], this->m_vertices[i].GetData(), m_vertices[i].GetElementSize());
			}
			this->m_upload_buffer = std::make_unique<GPUUploadBuffer>(device, this->m_vertices.size(), this->m_vertices[0].GetElementSize(), vertices);
			delete[] vertices;

			this->m_upload_buffer->GetResource()->SetName(L"Vertex Buffer Upload Buffer");
		}

		void CreateVertexBuffer(GraphicsDevice& device, GraphicsCommandList& commandList)
		{
			this->m_vertex_buffer = std::make_unique<GPUDefaultBuffer>(device, commandList, this->m_vertices.size(), this->m_vertices[0].GetElementSize());
			this->m_vertex_buffer->GetResource()->SetName(L"Vertex Buffer");
		}

		void UploadData(GraphicsCommandList& commandList)
		{
			this->m_upload_buffer->CopyDataToGPUBuffer(commandList, *this->m_vertex_buffer);
		}

		void CreateVertexBufferDescriptor()
		{
			D3D12_VERTEX_BUFFER_VIEW vertex_buffer_descriptor = {};
			vertex_buffer_descriptor.BufferLocation = this->m_vertex_buffer->GetGPUAddress();
			vertex_buffer_descriptor.SizeInBytes = (UINT)(this->m_vertices.size() * this->m_vertices[0].GetElementSize());
			vertex_buffer_descriptor.StrideInBytes = (UINT)(this->m_vertices[0].GetElementSize());

			this->m_vertex_buffer_descriptor = vertex_buffer_descriptor;
		}
	};
}
