#pragma once

#include "../Vertices/VertexBuffer.hpp"

namespace DXR
{

	struct GraphicsCommandList;
	template<typename VertexStruct> struct VertexBuffer;

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
		TLAS() = default;
	private:
	public:
	private:
	};

	struct BLAS
	{
	public:
		BLAS() = default;

		template<typename T>
		BLAS(const DXR::VertexBuffer<T>& vb)
		{
		}
	private:
	public:
	private:
	};

}