#pragma once

#include "ModelVertex.hpp"
#include <string>
#include "../Core/Components/Vertices/VertexBuffer.hpp"
#include <directxmath.h>
#include "../Core/Components/Resource/Texture/Texture.hpp"

namespace DXR
{
    
	struct GraphicsDevice;
	struct GraphicsCommandList;
    
    struct Material
    {
        XMFLOAT3 ambient_coefficient;
        XMFLOAT3 diffuse_coefficient;
        XMFLOAT3 specular_coefficient;
    };
    
    struct Submesh
    {
        UINT pad;
    };
    
	struct Model
	{
        public:
		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;
		XMFLOAT3 AABB[2] = {{0,0,0},{1,1,1}};
        public:
		Model() = default;
		Model(const std::vector<OBJVertex>& vertices, const std::vector<UINT>& indices);
		VertexBuffer<OBJVertex> GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
		IndexBuffer GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
        private:
		void DetermineAABB();
	};
    
	struct ModelLoader
	{
        public:
		static Model LoadOBJ(const std::string& filepath);
        private:
        std::string static DetermineFolder(const std::string& filepath);
	};
    
}