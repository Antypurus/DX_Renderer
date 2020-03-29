#pragma once

#include "../Vertices/VertexBuffer.hpp"
#include "../Vertices/IndexBuffer.hpp"

namespace DXR
{
	struct GraphicsCommandList;
	struct GraphicsDevice;
	template<typename VertexStruct> struct VertexBuffer;
	struct GPUDefaultBuffer;

	struct AccelerationStructure;
	struct TLAS;
	struct BLAS;

	struct AccelerationStructure
	{
	public:
	private:
	public:
	private:
	};

	struct TLAS
	{
	public:
	private:
		std::vector<BLAS> m_blas_instances;
	public:
		TLAS() = default;
	private:
	};

	struct BLAS
	{
	public:
	private:
		std::unique_ptr<GPUDefaultBuffer> m_scratch_buffer = nullptr;
		std::unique_ptr<GPUDefaultBuffer> m_blas_buffer = nullptr;
		D3D12_RAYTRACING_GEOMETRY_DESC m_blas_descripiton;
	public:
		BLAS() = default;

		template<typename T>
		BLAS(GraphicsDevice& Device, GraphicsCommandList& commandList,const DXR::VertexBuffer<T>& vb, const IndexBuffer& ib, const bool IsOpaque = true);
	private:
		void BuildBLAS(GraphicsDevice& Device, GraphicsCommandList& commandList);
	};

	template<typename T>
	inline BLAS::BLAS(GraphicsDevice& Device, GraphicsCommandList& commandList,const DXR::VertexBuffer<T>& vb, const IndexBuffer& ib, const bool IsOpaque)
	{
		m_blas_descripiton.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		m_blas_descripiton.Triangles.VertexBuffer.StartAddress = vb.GetVertexBufferGPUAddress();
		m_blas_descripiton.Triangles.VertexBuffer.StrideInBytes = vb.GetVertexStride();
		m_blas_descripiton.Triangles.VertexCount = vb.GetVertexCount();
		m_blas_descripiton.Triangles.VertexFormat = vb.GetVertexPositionFormat();
		m_blas_descripiton.Triangles.IndexBuffer = ib.GetIndexBufferGPUAddress();
		m_blas_descripiton.Triangles.IndexCount = ib.GetIndexCount();
		m_blas_descripiton.Triangles.IndexFormat = ib.IndexFormat;
		m_blas_descripiton.Triangles.Transform3x4 = 0;//Adding a manual transform is currently not support
		if (IsOpaque)
		{
			m_blas_descripiton.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		}else
		{
			m_blas_descripiton.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		}
		this->BuildBLAS(Device,commandList);
	}

}