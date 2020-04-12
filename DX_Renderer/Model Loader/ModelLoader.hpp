#pragma once

#include "ModelVertex.hpp"
#include <string>

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
	};

	struct OBJModelLoader
	{
	public:
		static OBJMesh Load(const std::string& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList);
	};

}