#pragma once
#include "../ThirdParty/tiny_obj_loader/tiny_obj_loader.h"
#include "ModelVertex.hpp"
#include <string>

namespace DXR
{

	struct GraphicsDevice;
	struct GraphicsCommandList;

	struct OBJModel
	{
	public:
		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;
	};

	struct OBJModelLoader
	{
	public:
		static OBJModel Load(const std::wstring& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList);
	};

}