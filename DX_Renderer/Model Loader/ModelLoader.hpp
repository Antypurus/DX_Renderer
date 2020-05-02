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
        std::string name = "";
        XMFLOAT3 ambient_coefficient = {1,1,1};
        XMFLOAT3 diffuse_coefficient = {1,1,1};
        XMFLOAT3 specular_coefficient = {0,0,0};
        bool has_texture = false;
        std::shared_ptr<Texture> texture = nullptr;//TODO(Tiago): Still think that this should be a unique ptr and not a shared ptr, but C++ is dumb sometimes
        
        Material() = default;
        Material(const Material& other);
    };
    
    struct Submesh
    {
        std::vector<UINT> indices;//TODO(Tiago): does the submess really need to store its indices? Seems kinda of like a waste of RAM
        Material* material;//NOTE(Tiago): non owning pointer, actual material is in the model
        std::unique_ptr<IndexBuffer> index_buffer = nullptr;
        
        Submesh(const std::vector<UINT>& indices, Material& material, GraphicsDevice& device, GraphicsCommandList& command_list);
        void Draw(GraphicsCommandList& command_list, UINT texture_slot, UINT sampler_slot);
    };
    
	struct Model
	{
        public:
		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;
        std::vector<Material> materials;
        std::vector<Submesh> submeshes;
		XMFLOAT3 AABB[2] = {{0,0,0},{1,1,1}};
        public:
		Model() = default;
		Model(const std::vector<OBJVertex>& vertices,
              const std::vector<UINT>& indices,
              const std::unordered_map<UINT,std::vector<UINT>>& submeshes,
              std::vector<Material>& materials);
		VertexBuffer<OBJVertex> GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
		IndexBuffer GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList);
        private:
		void DetermineAABB();
	};
    
	struct ModelLoader
	{
        public:
		static Model LoadOBJ(const std::string& filepath, GraphicsDevice& device, GraphicsCommandList& command_list);
        private:
        std::string static DetermineFolder(const std::string& filepath);
	};
    
}