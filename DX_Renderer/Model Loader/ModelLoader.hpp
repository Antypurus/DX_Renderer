#pragma once

#include "ModelVertex.hpp"
#include <string>

#include "../Core/Components/Vertices/VertexBuffer.hpp"

namespace DXR
{

	struct GraphicsDevice;
	struct GraphicsCommandList;

	struct OBJMesh
	{
	public:
		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;
	public:
		OBJMesh() = default;
		OBJMesh(const std::vector<OBJVertex>& vertices, const std::vector<UINT>& indices);
		VertexBuffer<OBJVertex> GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
		IndexBuffer GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
	};

	struct OBJModelLoader
	{
	public:
		static OBJMesh Load(const std::string& filepath);
	};

}